$currentDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$program = $currentDir + "\..\build\bin\TSP_IMPROVER"
$dataPath = $currentDir + "\..\data\tsp_instances\"
$solutionPath = $currentDir + "\..\data\results\"
$solutionFiles = (Get-ChildItem "$solutionPath" -Exclude "*ls*" | Get-ChildItem -Include "*r*.txt").FullName
$disturbSizes = @(5, 10, 20, 30, 50, 75)
# 'o' denotes no local search improver
$subImprovers = @('o', 'p')
$paramList = @()

$datasetTimes = [System.Collections.Generic.Dictionary[string, int]]::new()
$datasetTimes.Add("TSPA", 9120000)
$datasetTimes.Add("TSPB", 8620000)
$datasetTimes.Add("TSPC", 6500000)
$datasetTimes.Add("TSPD", 5400000)

foreach ($solution in $solutionFiles) {
    foreach ($disturb in $disturbSizes) {
        foreach ($improver in $subImprovers) {
            $solutionFilename = Split-Path $solution -Leaf
            $dataset = $solutionFilename -replace "-.*", ".csv"
            $datesetPath = Join-Path $dataPath $dataset
            $paramList += [PSCustomObject]@{
                dataset  = $datesetPath
                improver = $improver
                solution = $solution
                disturb  = $disturb
            }
        }
    }
}

Write-Output "Processing $($paramList.Count) solutions"

$paramList | ForEach-Object -Parallel {
    $dataset = $_.dataset
    $improver = $_.improver
    $solution = $_.solution
    $disturb = $_.disturb
    $solutionFilename = Split-Path $solution -Leaf
    $solutionFilenameNoExtension = $solutionFilename -replace ".txt", ""
    $solutionConfig = $solutionFilenameNoExtension.Substring(5)
    $datasetName = $solutionFilenameNoExtension.Substring(0, 4)
    $runningTime = $($using:datasetTimes)[$datasetName]

    $outFileName = "$datasetName-lse$improver-$disturb-$solutionConfig.txt"
    $outFilePath = Join-Path $($using:solutionPath) $outFileName
    $ecpArgs = "-f ""$dataset"" -s ""$solution"" -o ""$outFilePath"" -t e -n edge -st $improver -sp1 $runningTime -sp2 $disturb"
    Write-Output "Processing $solutionFilename with $improver"
    $status = Start-Process $($using:program) -ArgumentList $ecpArgs -Wait -WindowStyle Hidden -PassThru
    if ($status.ExitCode -ne 0) {
        Write-Error "Error calling program with: $ecpArgs"
        Write-Error "Error processing $solution with $improver"
        Write-Error $status
    }
} -ThrottleLimit 48
