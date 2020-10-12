$account = "lintest"
$project = "AddinTemplate"
$name = "NativeAddIn"

#https://ci.appveyor.com/api-keys
$token = $env:API_TOKEN 
$url1c = $env:URL_1CV8T
$path = $env:APPVEYOR_BUILD_FOLDER
$version = $env:APPVEYOR_BUILD_VERSION
$postfix = '_' + $version -replace '\.', '_'
Update-AppveyorBuild -Version "$version"
Write-Output "Version: $version"

$apiUrl = 'https://ci.appveyor.com/api'
$headers = @{
  "Authorization" = "Bearer $token"
  "Content-type"  = "application/json"
}

$project = Invoke-RestMethod -Method Get -Uri "$apiUrl/projects/$account/$project/build/$version" -Headers $headers

$jobId = $project.build.jobs[0].jobId
$artifacts = Invoke-RestMethod -Method Get -Uri "$apiUrl/buildjobs/$jobId/artifacts" -Headers $headers
$artifactFileName = $artifacts[0].fileName

Invoke-RestMethod -Method Get -Uri "$apiUrl/buildjobs/$jobId/artifacts/$artifactFileName" `
  -OutFile "$path\Linux.zip" -Headers @{ "Authorization" = "Bearer $token" }

Expand-Archive -Force -Path "$path\Linux.zip" -DestinationPath $path

Rename-Item "$path\lib${name}Win32.dll" "${name}Win32$postfix.dll"
Rename-Item "$path\lib${name}Win64.dll" "${name}Win64$postfix.dll"
Rename-Item "$path\lib${name}Lin32.so" "${name}Lin32$postfix.so"
Rename-Item "$path\lib${name}Lin64.so" "${name}Lin64$postfix.so"

$compress = @{
  Path            = "$path\$name*.dll", "$path\$name*.so", "$path\manifest.xml"
  DestinationPath = "$path\AddIn.zip"
}
Compress-Archive @compress

New-Item -ItemType Directory -Force -Path "$path\Example\Templates\$name\" | Out-Null
New-Item -ItemType Directory -Force -Path "$path\Example\Templates\$name\Ext\" | Out-Null
Copy-Item -Path "$path\AddIn.zip" -Destination "$path\Example\Templates\$name\Ext\Template.bin"

$dist1c = "$path\1cv8.zip"
if (!(Test-Path $dist1c)) {
  Write-Host "Download 1cv8.zip ..."
  Invoke-WebRequest -Uri $url1c -OutFile $dist1c
}

Write-Host "Expand archive 1cv8.zip ..."
Expand-Archive -Force -Path $dist1c -DestinationPath $path
Set-Content "$path\bin\conf\conf.cfg" "DisableUnsafeActionProtection=.*;"

& "bin\1cv8t.exe" DESIGNER /F "$path\Data" /LoadExternalDataProcessorOrReportFromFiles "Example.xml" "$name.epf"  /Out"Example.log"

Write-Host "Process autotest..."
#Start-Process "bin\1cv8ct.exe" -ArgumentList "/F $path\Autobase" -Wait
