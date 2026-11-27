param (
    [ValidateSet('x86','arm')][string]$arch = 'x86',
    [ValidateSet('win64','win32')][string]$target = 'win64',
    [string]$config,
    [string]$buildRoot,
    [string]$qtRoot = 'Qt'
)

# https://stackoverflow.com/a/48999101
Set-StrictMode -Version latest
$ErrorActionPreference = "Stop"
function Exec {
    param(
        [Parameter(Position=0,Mandatory=1)][scriptblock]$cmd
    )
    Write-Host ("> " + $cmd.ToString().Trim())
    & $cmd
    if ($LastExitCode -ne 0) {
        throw
    }
}

$qtVersion = '6.8.3'
$qtToolchain = 'msvc2022_64'

$qtToolchainPath = ''
$gui = 'OFF'

if ($arch -eq 'x86') {
    if ($target -eq 'win64') {
        $toolset = 'x64'

        $qtToolchainPath = "$qtRoot\$qtVersion\$qtToolchain"
        if (!(Test-Path $qtToolchainPath -PathType Container)) {
            $qtVersionDotless = $qtVersion -replace '\.'
            $baseUrl = "https://download.qt.io/online/qtsdkrepository/windows_x86/desktop/qt6_$qtVersionDotless/qt6_$qtVersionDotless"
            $packageUrl = "$baseUrl/qt.qt6.$qtVersionDotless.win64_$qtToolchain"
            $tag = "0-202503201308"
            $suffix = "Windows-Windows_11_23H2-MSVC2022-Windows-Windows_11_23H2-X86_64"

            # windeployqt is part of qtbase since Qt 6.5
            $modules = @('qtbase')

            $archiveUrls = @()
            foreach ($module in $modules) {
                $archiveUrls += "$packageUrl/$qtVersion-${tag}${module}-${suffix}.7z"
            }

            # Debug symbols (PDBs) are only useful for local debugging, and big
            if ($Env:GITHUB_ACTIONS -ne 'true') {
                $debugPackageUrl = "$baseUrl/qt.qt6.$qtVersionDotless.debug_info.win64_$qtToolchain"
                foreach ($module in $modules) {
                    $archiveUrls += "$debugPackageUrl/$qtVersion-${tag}${module}-${suffix}-debug-symbols.7z"
                }
            }

            foreach ($archiveUrl in $archiveUrls) {
                $archive = $archiveUrl.Split('/')[-1]

                if (!(Test-Path $archive -PathType Leaf)) {
                    Write-Host "Downloading $archiveUrl ..."
                    # https://github.com/PowerShell/PowerShell/issues/2896
                    do {
                        try {
                            Invoke-WebRequest -Uri $archiveUrl -OutFile $archive -UserAgent NativeHost
                            $retry = $false
                        }
                        catch {
                            if (($_.Exception.GetType() -match "HttpResponseException") -and ($_.Exception -match "302")) {
                                $archiveUrl = $_.Exception.Response.Headers.Location.AbsoluteUri
                                Write-Host "Redirected to $archiveUrl ..."
                                $retry = $true
                            }
                            else {
                                throw $_
                            }
                        }
                    } while ($retry)
                }

                # Unlike Qt5, Qt6 archives are relative to the toolchain directory
                Write-Host "Extracting $archive to $qtToolchainPath ..."
                Exec { 7z x -y "-o$qtToolchainPath" $archive | Out-Null }
            }
        }

        $qtToolchainPath = (Resolve-Path $qtToolchainPath).ProviderPath
        $qtBinPath = "$qtToolchainPath\bin"

        $gui = 'ON'
    } else {
        # There are no official Qt6 binaries for 32-bit Windows
        $toolset = 'Win32'
    }
} else {
    # $arch = 'arm'
    if ($target -eq 'win64') {
        $toolset = 'ARM64'
    } else {
        $toolset = 'ARM'
    }
}

# Use Mesa's llvmpipe software OpenGL implementation to run the OpenGL tests,
# so that results don't depend on the machine's GPU (or lack thereof).  The
# tracer and glretrace honor TRACE_LIBGL.
#
# Binaries from https://github.com/mmozeiko/build-mesa/releases (single-DLL
# llvmpipe builds, statically linked, no extra DLL dependencies).
#
# It can also be installed system-wide as the software OpenGL ICD (MSOGL), see
# https://docs.mesa3d.org/drivers/llvmpipe.html#windows
$traceLibGl = $Env:TRACE_LIBGL
if ($arch -eq 'x86' -and !$traceLibGl) {
    $mesaVersion = '26.3.1'
    $mesaDigests = @{
        x64 = '08145a3717e0c1be0d97b74b53070f14eb39cb2f7de22f84fe20d1a581299c12'
        x86 = 'af82e5ec8d8b6e89ccc662bfafecc2f14587964d6ec56833af4ab22b2808b370'
    }
    $mesaArch = if ($target -eq 'win64') { 'x64' } else { 'x86' }
    $mesaArchive = "mesa-llvmpipe-$mesaArch-$mesaVersion.7z"
    $mesaArchivePath = "downloads\$mesaArchive"
    $mesaDir = "downloads\mesa\$mesaArch"

    if (!(Test-Path "$mesaDir\opengl32.dll" -PathType Leaf)) {
        New-Item -ItemType Directory -Force downloads | Out-Null

        if (!(Test-Path $mesaArchivePath -PathType Leaf)) {
            $mesaUrl = "https://github.com/mmozeiko/build-mesa/releases/download/$mesaVersion/$mesaArchive"
            Write-Host "Downloading $mesaUrl ..."
            Invoke-WebRequest -Uri $mesaUrl -OutFile $mesaArchivePath
        }

        $mesaDigest = (Get-FileHash $mesaArchivePath -Algorithm SHA256).Hash.ToLower()
        if ($mesaDigest -ne $mesaDigests[$mesaArch]) {
            Remove-Item $mesaArchivePath
            throw "SHA-256 mismatch for ${mesaArchive}: expected $($mesaDigests[$mesaArch]), got $mesaDigest"
        }

        Write-Host "Extracting $mesaArchive to $mesaDir ..."
        Exec { 7z x -y "-o$mesaDir" $mesaArchivePath opengl32.dll | Out-Null }
    }

    $traceLibGl = (Resolve-Path "$mesaDir\opengl32.dll").ProviderPath
}
$extraCmakeArgs = @()
if ($traceLibGl) {
    # Passed to CMake, which propagates it to the tests' environment, so that
    # ctest can also be run directly.
    Write-Host "Using TRACE_LIBGL=$traceLibGl."
    $extraCmakeArgs += "-DTRACE_LIBGL=$traceLibGl"
}

# The tests need PIL (a.k.a. pillow) to compare snapshots against reference
# images.  Make sure CMake uses the very same interpreter we install it into, as
# there can be several Pythons around (e.g., on GitHub runners).
$python = (Get-Command python).Source
& $python -c "import PIL" 2>$null
if ($LASTEXITCODE -ne 0) {
    Write-Host "Installing pillow into $python ..."
    Exec { & $python -m pip install --disable-pip-version-check pillow }
}
$extraCmakeArgs += "-DPython3_EXECUTABLE=$python"

# https://github.com/microsoft/vswhere/wiki/Find-VC#powershell
$product = & "${Env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -format json | ConvertFrom-Json | Select-Object -First 1
$productVersion = [System.Version]$product.installationVersion
if ($product.DisplayName -notmatch '^.* (?<year>\d\d\d\d)$') {
	throw
}
$generator = "Visual Studio $($productVersion.Major) $($Matches.year)"

if (!$config) {
    if ($arch -eq 'arm') {
        # Avoid fatal error LNK1322: cannot avoid potential ARM hazard (Cortex-A53 MPCore processor bug 843419)
        $config = 'RelWithDebInfo'
    } else {
        $config = 'Debug'
    }
    # Default to release on master and tags for GitHub builds
    if ($Env:GITHUB_EVENT_NAME -eq "push" -And ($Env:GITHUB_REF -eq 'refs/heads/master' -Or $Env:GITHUB_REF.StartsWith('refs/tags/'))) {
        $config = 'RelWithDebInfo'
    }
}

if (!$buildRoot) {
    if ($arch -eq 'x86') {
	    $buildRoot = "build\$target"
    } else {
	    $buildRoot = "build\$target-$arch"
    }
}
Write-Host "Configuring onto $buildRoot ..."
Exec { cmake "-S." "-B$buildRoot" -G $generator -A $toolset "-DCMAKE_SYSTEM_VERSION=10.0.19041.0" "-DCMAKE_PREFIX_PATH=$qtToolchainPath" "-DENABLE_GUI=$gui" -DENABLE_QT6=1  -DENABLE_TESTS=1 @extraCmakeArgs}

if (!$Env:CTEST_PARALLEL_LEVEL) {
    $Env:CTEST_PARALLEL_LEVEL = [Environment]::ProcessorCount
}

$targets = @('--target', 'ALL_BUILD', '--target', 'check')
if ($Env:PACKAGE -eq 'true' -Or $Env:GITHUB_ACTIONS -eq 'true') {
    $targets += @('--target', 'package')
}
if ($Env:STRESS_ITERATIONS) {
    # Throwaway flakiness hunt: just build, then repeat a few tests many times.
    $targets = @('--target', 'ALL_BUILD')
}

Write-Host "Building ..."
Exec { cmake --build $buildRoot --config $config @targets "--" /verbosity:minimal /maxcpucount }

if ($Env:STRESS_ITERATIONS) {
    $iterations = [int]$Env:STRESS_ITERATIONS
    $failures = @{}
    $failedIterations = 0
    $ErrorActionPreference = "Continue"
    for ($i = 1; $i -le $iterations; $i++) {
        Write-Host "::group::iteration $i"
        $out = & ctest --test-dir $buildRoot -C $config -R $Env:STRESS_REGEX -j 2 --output-on-failure 2>&1 | ForEach-Object { "$_" }
        $code = $LASTEXITCODE
        $out | Write-Host
        Write-Host "::endgroup::"
        if ($code -ne 0) {
            $failedIterations++
            Write-Host "iteration $i FAILED (ctest exit code $code)"
            foreach ($m in ($out | Select-String -Pattern 'Test +#\d+: (\S+) \.+\*\*\*(\w+)')) {
                $key = "$($m.Matches[0].Groups[1].Value) ($($m.Matches[0].Groups[2].Value))"
                $failures[$key] = 1 + [int]$failures[$key]
            }
        }
    }
    Write-Host "==== $failedIterations of $iterations iterations failed ===="
    $failures.GetEnumerator() | Sort-Object Name | ForEach-Object { Write-Host "$($_.Name): $($_.Value)" }
    if ($failedIterations -ne 0) {
        exit 1
    }
}
