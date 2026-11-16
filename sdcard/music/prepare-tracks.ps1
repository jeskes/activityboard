$ParentDir = Split-Path (Resolve-Path ".") -Parent
$SoundBaseDir = Join-Path $ParentDir "sound"
$CoreBaseDir  = Join-Path $ParentDir "core"

# iterate direct subdirectories
$SubDirs = Get-ChildItem -Directory | Sort-Object Name

foreach ($Dir in $SubDirs) {
    # 1. Ziel-Unterverzeichnis in "../sound/<Ordnername>" prüfen, leeren oder neu anlegen
    $TargetSoundDir = Join-Path $SoundBaseDir $Dir.Name
    if (Test-Path $TargetSoundDir) {
        # Alle vorhandenen Dateien und Unterordner darin löschen
        Remove-Item -Path (Join-Path $TargetSoundDir "*") -Recurse -Force
    } else {
        New-Item -ItemType Directory -Path $TargetSoundDir -Force | Out-Null
    }

    # 2. Ziel-Unterverzeichnis in "../core/<Ordnername>" anlegen
    $TargetCoreDir = Join-Path $CoreBaseDir $Dir.Name
    if (-not (Test-Path $TargetCoreDir)) {
        New-Item -ItemType Directory -Path $TargetCoreDir -Force | Out-Null
    }

    # Alle MP3-Dateien im aktuellen Unterordner alphabetisch sortiert holen
    $Mp3Files = Get-ChildItem -Path $Dir.FullName -Filter "*.mp3" -File | Sort-Object Name

    # Ordered-Dictionary für das Mapping
    $Mapping = [ordered]@{}
    $Index = 1

    foreach ($File in $Mp3Files) {
        # Index dreistellig mit Nullen auffüllen (z.B. 1 -> "001")
        $PaddedIndex = $Index.ToString("000")
		$Name = if ($File.BaseName -match '^\d{3}_(.*)$') {
			$Matches[1]
		}
		else {
			$File.BaseName
		}
        $NewFileName = "${PaddedIndex}_${Name}.mp3"
        $DestFilePath = Join-Path $TargetSoundDir $NewFileName

        # MP3-Datei nach "../sound/<Ordnername>/001.mp3" kopieren
        Copy-Item -Path $File.FullName -Destination $DestFilePath -Force

        # Mapping: "Dateiname ohne Endung" -> Index (als Zahl)
        $Mapping[$File.BaseName] = $Index

        Write-Host "[$($Dir.Name)] $($File.Name) -> $NewFileName (Index: $Index)"
        $Index++
    }

    # music.json in "../core/<Ordnername>/music.json" speichern (überschreibt alte Datei automatisch)
    $JsonPath = Join-Path $TargetCoreDir "music.json"
    $JsonContent = if ($Mapping.Count -gt 0) {
        $Mapping | ConvertTo-Json
    } else {
        "{}" | Set-Content -Path $JsonPath -Encoding UTF8
    }

	$Utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($JsonPath, $JsonContent, $Utf8NoBom)
}

Write-Host "`nDone." -ForegroundColor Green
Write-Host "tracks copied to:  $SoundBaseDir"
Write-Host "configuration files copied to: $CoreBaseDir"
