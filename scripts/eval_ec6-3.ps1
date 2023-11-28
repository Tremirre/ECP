$currentDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$program = $currentDir + "\..\build\bin\TSP_IMPROVER"
$dataPath = $currentDir + "\..\data\tsp_instances\"
$solutionPath = $currentDir + "\..\data\results\"
$solutionFiles = (Get-ChildItem "$solutionPath" -Exclude "*ls*" | Get-ChildItem -Include "*r*.txt").FullName
$peturbSizes = @(3, 5, 10, 20, 30, 50)
$paramList = @()

$mslsFiles = (Get-ChildItem "$solutionPath" | Get-ChildItem -Include "*lsm*.txt").FullName
$datasetTimes = [System.Collections.Generic.Dictionary[string, int]]::new()
$datasetTimes.Add("TSPA", 0)
$datasetTimes.Add("TSPB", 0)
$datasetTimes.Add("TSPC", 0)
$datasetTimes.Add("TSPD", 0)

$mslsFiles | ForEach-Object {
    $fileName = Split-Path $_ -Leaf
    $dataset = $fileName.Substring(0, 4)
    $content = Get-Content $_
    $time = [int]$content[-1]
    $datasetTimes[$dataset] += [int]($time / ($mslsFiles.Count / 4))
}

foreach ($solution in $solutionFiles) {
    foreach ($peturbSize in $peturbSizes) {
        $solutionFilename = Split-Path $solution -Leaf
        $dataset = $solutionFilename -replace "-.*", ".csv"
        $datesetPath = Join-Path $dataPath $dataset
        $paramList += [PSCustomObject]@{
            dataset  = $datesetPath
            improver = 'i'
            solution = $solution
            peturb   = $peturbSize
        }
    }
}

Write-Output "Processing $($paramList.Count) solutions"

$paramList | ForEach-Object -Parallel {
    $dataset = $_.dataset
    $improver = $_.improver
    $solution = $_.solution
    $peturb = $_.peturb
    $solutionFilename = Split-Path $solution -Leaf
    $solutionFilenameNoExtension = $solutionFilename -replace ".txt", ""
    $solutionConfig = $solutionFilenameNoExtension.Substring(5)
    $datasetName = $solutionFilenameNoExtension.Substring(0, 4)
    $runningTime = $($using:datasetTimes)[$datasetName]

    $outFileName = "$datasetName-ls$improver-$peturb-$solutionConfig.txt"
    $outFilePath = Join-Path $($using:solutionPath) $outFileName
    $ecpArgs = "-f ""$dataset"" -s ""$solution"" -o ""$outFilePath"" -t $improver -n edge -st p -sp1 $runningTime -sp2 $peturb"
    Write-Output "Processing $solutionFilename with $improver"
    $status = Start-Process $($using:program) -ArgumentList $ecpArgs -Wait -WindowStyle Hidden -PassThru
    if ($status.ExitCode -ne 0) {
        Write-Error "Error calling program with: $ecpArgs"
        Write-Error "Error processing $solution with $improver"
        Write-Error $status
    }
} -ThrottleLimit 48
