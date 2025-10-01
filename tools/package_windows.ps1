<#
.SYNOPSIS
    Create Windows release artefacts for the Simple OpenTTD Client.

.DESCRIPTION
    This script gathers the built client binaries, runtime dependencies, and
    documentation into both ZIP and installer payloads. It is designed to run on
    a Windows host with the Visual Studio 2022 toolchain and vcpkg installed.

    The implementation currently focuses on the ZIP package while providing
    extension points for an NSIS-based installer. Phase 3 contributors can flesh
    out the `Invoke-InstallerBuild` function once the installer layout is final.

.PARAMETER Configuration
    CMake configuration to package (defaults to Release).

.PARAMETER OutputDirectory
    Destination directory for the generated artefacts. Will be created if it
    does not exist.

.EXAMPLE
    PS> ./tools/package_windows.ps1 -Configuration Release -OutputDirectory dist
#>
[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release',

    [string]$OutputDirectory = 'dist'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-BinaryDirectory {
    param(
        [string]$Config
    )

    $buildRoot = Join-Path -Path $PSScriptRoot -ChildPath '..\\build'
    $binDir = Join-Path -Path $buildRoot -ChildPath "$Config\\bin"
    if (-not (Test-Path -Path $binDir)) {
        throw "Expected binaries in '$binDir'. Build the project before packaging."
    }
    return (Resolve-Path -Path $binDir).Path
}

function Get-Dependencies {
    param(
        [string]$BinaryDir
    )

    $deps = @()
    $deps += Get-ChildItem -Path $BinaryDir -Filter '*.dll' -File | Select-Object -ExpandProperty FullName
    return $deps
}

function New-ZipPackage {
    param(
        [string]$BinaryDir,
        [string]$OutputDir
    )

    $zipName = "SimpleOpenTTDClient-$Configuration-win64.zip"
    $zipPath = Join-Path -Path $OutputDir -ChildPath $zipName

    if (Test-Path -Path $zipPath) {
        Remove-Item -Path $zipPath -Force
    }

    $temp = New-TemporaryFile
    Remove-Item $temp -Force
    New-Item -ItemType Directory -Path $temp | Out-Null

    Copy-Item -Path (Join-Path $BinaryDir 'sotc.exe') -Destination $temp
    foreach ($dll in (Get-Dependencies -BinaryDir $BinaryDir)) {
        Copy-Item -Path $dll -Destination $temp
    }

    Copy-Item -Path (Join-Path $PSScriptRoot '..\\README.md') -Destination $temp
    Copy-Item -Path (Join-Path $PSScriptRoot '..\\docs\\RELEASE_NOTES_0.1.0.md') -Destination $temp

    Compress-Archive -Path (Join-Path $temp '*') -DestinationPath $zipPath
    Remove-Item -Path $temp -Recurse -Force

    Write-Host "Created $zipPath"
}

function Invoke-InstallerBuild {
    param(
        [string]$BinaryDir,
        [string]$OutputDir
    )

    $installerName = "SimpleOpenTTDClient-$Configuration-Setup.exe"
    $installerPath = Join-Path -Path $OutputDir -ChildPath $installerName

    Write-Warning "Installer packaging is not yet implemented. Stub created at $installerPath"
    Set-Content -Path $installerPath -Value "Installer build pending."
}

$binaryDir = Get-BinaryDirectory -Config $Configuration
$resolvedOutput = Resolve-Path -Path (New-Item -ItemType Directory -Path $OutputDirectory -Force)

New-ZipPackage -BinaryDir $binaryDir -OutputDir $resolvedOutput
Invoke-InstallerBuild -BinaryDir $binaryDir -OutputDir $resolvedOutput

Write-Host 'Packaging complete.'
