$currentDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$program = $currentDir + "\..\build\bin\TSP_IMPROVER"
$dataPath = $currentDir + "\..\data\tsp_instances\"
$solutionPath = $currentDir + "\..\data\results\"
$solutionFiles = (Get-ChildItem "$solutionPath" -Exclude "*ls*" | Get-ChildItem -Include "*r*.txt").FullName
$paramList = @()

$idx = 0
foreach ($solution in $solutionFiles) {
    $solutionFilename = Split-Path $solution -Leaf
    $dataset = $solutionFilename -replace "-.*", ".csv"
    $datesetPath = Join-Path $dataPath $dataset
    $paramList += [PSCustomObject]@{
        dataset  = $datesetPath
        improver = 'm'
        solution = $solution
        index    = $idx++
    }
}

Write-Output "Processing $($paramList.Count) solutions"

$paramList | ForEach-Object -Parallel {
    $dataset = $_.dataset
    $improver = $_.improver
    $solution = $_.solution
    $index = $_.index
    $solutionFilename = Split-Path $solution -Leaf
    $solutionFilenameNoExtension = $solutionFilename -replace ".txt", ""
    $solutionConfig = $solutionFilenameNoExtension.Substring(5)
    $datasetName = $solutionFilenameNoExtension.Substring(0, 4)
    $outFileName = "$datasetName-ls$improver-$solutionConfig.txt"
    $outFilePath = Join-Path $($using:solutionPath) $outFileName
    $ecpArgs = "-f ""$dataset"" -s ""$solution"" -o ""$outFilePath"" -t $improver -n edge -st p -sp1 $index -sp2 200"
    Write-Output "Processing $solutionFilename with $improver"
    $status = Start-Process $($using:program) -ArgumentList $ecpArgs -Wait -WindowStyle Hidden -PassThru
    if ($status.ExitCode -ne 0) {
        Write-Error "Error calling program with: $ecpArgs"
        Write-Error "Error processing $solution with $improver"
        Write-Error $status
    }
} -ThrottleLimit 48
