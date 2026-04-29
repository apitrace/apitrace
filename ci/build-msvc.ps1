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

$qtVersion = '5.15.2'
$qtVersionDotless = $qtVersion -replace '\.'

if ($arch -eq 'x86') {
    if ($target -eq 'win64') {
        $qtToolchain = 'msvc2019_64'
        $qtArch = 'X86_64'
        $toolset = 'x64'
    } else {
        $qtToolchain = 'msvc2019'
        $qtArch = 'X86'
        $toolset = 'Win32'
    }

    $baseUrl = "https://download.qt.io/online/qtsdkrepository/windows_x86/desktop/qt5_$qtVersionDotless"

    $packageUrl = "$baseUrl/qt.qt5.$qtVersionDotless.${target}_$qtToolchain"

    $tag = "0-202011130602"

    $suffix = "Windows-Windows_10-MSVC2019-Windows-Windows_10"

    $modules = @('qtbase', 'qttools')

    $qtToolchainPath = "$qtRoot\$qtVersion\$qtToolchain"
    if (!(Test-Path $qtToolchainPath -PathType Container)) {

        foreach ($module in $modules) {
            $archive = "$qtVersion-${tag}${module}-${suffix}-$qtArch.7z"
            $archiveUrl = "$packageUrl/$archive"

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

            Write-Host "Extracting $archive to $qtToolchainPath ..."
            Exec { 7z x -y "-o$qtRoot" $archive | Out-Null }
        }
    }

    $qtBinPath = Resolve-Path "$qtToolchainPath\bin"
    Write-Host "Adding $qtBinPath to environment path."
    $Env:Path = "$qtBinPath;$Env:Path"

    $gui = 'ON'
} else {
    # $arch = 'arm'
    if ($target -eq 'win64') {
        $toolset = 'ARM64'
    } else {
        $toolset = 'ARM'
    }
    $qtToolchainPath = ''
    $gui = 'OFF'
}

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
Exec { cmake "-S." "-B$buildRoot" -G $generator -A $toolset "-DCMAKE_SYSTEM_VERSION=10.0.19041.0" "-DCMAKE_PREFIX_PATH=$qtToolchainPath" "-DENABLE_GUI=$gui" }

Write-Host "Building ..."
Exec { cmake --build $buildRoot --config $config --target ALL_BUILD --target check --target package "--" /verbosity:minimal /maxcpucount }
