$currentDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$program = $currentDir + "\..\build\bin\TSP_SOLVER"
$dataPath = $currentDir + "\..\data\tsp_instances\"
$outputPath = $currentDir + "\..\data\results\"


$percentage = 50
$startIndices = 0..19
$datasets = (Get-ChildItem $dataPath -Filter *.csv).FullName
$triplesList = @()

foreach ($dataset in $datasets) {
    foreach ($idx in $startIndices) {
        $triplesList += [PSCustomObject]@{
            dataset = $dataset
            solver  = 'r'
            idx     = $idx
        }
    }
}


$triplesList | ForEach-Object -Parallel {
    $dataset = $_.dataset
    $solver = $_.solver
    $idx = $_.idx
    $fileName = Split-Path $dataset -Leaf
    $fileNameNoExtension = $fileName -replace ".csv", ""
    $outFileName = "$fileNameNoExtension-$solver-$idx.txt"
    $outFilePath = Join-Path $($using:outputPath) $outFileName
    $ecpArgs = "-f ""$dataset"" -s $solver -i $idx -p $($using:percentage) -o ""$outFilePath"""
    Write-Output "Processing $fileName with $solver and $idx"
    $status = Start-Process $($using:program) -ArgumentList $ecpArgs -Wait -WindowStyle Hidden -PassThru
    if ($status.ExitCode -ne 0) {
        Write-Error "Error processing $dataset with $solver and $idx"
        Write-Error $status
    }
} -ThrottleLimit 48
