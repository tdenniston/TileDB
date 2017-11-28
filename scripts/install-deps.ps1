<#
.SYNOPSIS
This is a Powershell script to install TileDB dependencies on Windows.

.DESCRIPTION
This is a Powershell script to install TileDB dependencies on Windows.

.LINK
https://github.com/TileDB-Inc/TileDB

#>

[CmdletBinding()]
Param(
)

# Return the directory containing this script file.
function Get-ScriptsDirectory {
    Split-Path -Parent $PSCommandPath
}

$TileDBRootDirectory = Split-Path -Parent (Get-ScriptsDirectory)
$InstallPrefix = Join-Path $TileDBRootDirectory "deps-install"
$DownloadDirectory = Get-ScriptsDirectory
$DownloadZlibDest = Join-Path $DownloadDirectory "zlib.zip"
$DownloadLz4Dest = Join-Path $DownloadDirectory "lz4.zip"

function DownloadFile([string] $URL, [string] $Dest) {
    Write-Host "Downloading $URL to $Dest..."
    Invoke-WebRequest -Uri $URL -OutFile $Dest
}

function DownloadIfNotExists([string] $Path, [string] $URL) {
    if (!(Test-Path $Path)) {
	DownloadFile $URL $Path
    }
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
function Unzip([string] $Zipfile, [string] $Dest) {
    Write-Host "Extracting $Zipfile to $Dest..."
    [System.IO.Compression.ZipFile]::ExtractToDirectory($Zipfile, $Dest)
}

function Install-Zlib {
    $ZlibRoot = (Join-Path $DownloadDirectory "zlib-1.2.11")
    if (!(Test-Path $ZlibRoot)) {
	DownloadIfNotExists $DownloadZlibDest "https://zlib.net/zlib1211.zip"
	Unzip $DownloadZlibDest $DownloadDirectory
    }
    Push-Location
    Set-Location $ZlibRoot
    if (!(Test-Path build)) {
	New-Item -ItemType Directory -Path build
    }
    Set-Location build
    cmake -DCMAKE_INSTALL_PREFIX="$InstallPrefix" ..
    cmake --build . --config Release --target INSTALL
    Pop-Location
}

function Install-LZ4 {
    $Lz4Root = (Join-Path $DownloadDirectory "lz4")
    if (!(Test-Path $Lz4Root)) {
	DownloadIfNotExists $DownloadLz4Dest "https://github.com/lz4/lz4/releases/download/v1.8.0/lz4_v1_8_0_win64.zip"
	New-Item -ItemType Directory -Path $Lz4Root
	Unzip $DownloadLz4Dest $Lz4Root
    }
    $DllDir = Join-Path $Lz4Root "dll"
    Copy-Item (Join-Path $DllDir "liblz4.lib") (Join-Path (Join-Path $InstallPrefix "lib") "lz4.lib")
    Copy-Item (Join-Path $DllDir "liblz4.so.1.8.0.dll") (Join-Path (Join-Path $InstallPrefix "bin") "lz4.dll")
    $IncDir = Join-Path $Lz4Root "include"
    Copy-Item (Join-Path $IncDir "*") (Join-Path $InstallPrefix "include")
}

function Install-All-Deps {
    Install-Zlib
    Install-LZ4
}

Install-All-Deps

Write-Host "Finished."
