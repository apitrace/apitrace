Utilities for injection of DLLs in Windows.

Wrapper DLLs work well for self-contained APIs (e.g., OpenGL with its
opengl32.dll), but not for APIs which are spread across multiple DLLs, in
particular DXGI and D3D10+ APIs.
