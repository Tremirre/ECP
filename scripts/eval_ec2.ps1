$currentDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$program = $currentDir + "\..\build\bin\ECP"
$dataPath = $currentDir + "\..\data\tsp_instances\"
$outputPath = $currentDir + "\..\data\results\"


$percentage = 50
$startIndices = 0..199
$weights = 0..8 | ForEach-Object { $_ / 4 }
$solver = "d"

$datasets = (Get-ChildItem $dataPath -Filter *.csv).FullName
$triplesList = @()

foreach ($dataset in $datasets) {
    foreach ($weight in $weights) {
        foreach ($idx in $startIndices) {
            $triplesList += [PSCustomObject]@{
                dataset = $dataset
                weight  = $weight
                idx     = $idx
            }
        }
    }
}


$triplesList | ForEach-Object -Parallel {
    $dataset = $_.dataset
    $weight = $_.weight
    $idx = $_.idx
    $fileName = Split-Path $dataset -Leaf
    $fileNameNoExtension = $fileName -replace ".csv", ""
    $outFileName = "$fileNameNoExtension-$($using:solver)$weight-$idx.txt"
    $outFilePath = Join-Path $($using:outputPath) $outFileName
    $ecpArgs = "-f ""$dataset"" -s $($using:solver) -i $idx -p $($using:percentage) -o ""$outFilePath"" -w $weight"
    Write-Output "Processing $fileName with $($using:solver) w:$weight and $idx"
    $status = Start-Process $($using:program) -ArgumentList $ecpArgs -Wait -WindowStyle Hidden -PassThru
    if ($status.ExitCode -ne 0) {
        Write-Error "Error processing $dataset with $($using:solver) w:$weight and $idx"
        Write-Error $status
    }
} -ThrottleLimit 48
