$currentDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$program = $currentDir + "\..\build\bin\TSP_IMPROVER"
$dataPath = $currentDir + "\..\data\tsp_instances\"
$solutionPath = $currentDir + "\..\data\results\"
$improvers = @("g", "s")
$ntypes = @("both", "edge", "node")
$solutionFiles = (Get-ChildItem "$solutionPath" -Exclude "*ls*" | Get-ChildItem -Include "*.txt").FullName
$quadriplesList = @()

foreach ($improver in $improvers) {
    foreach ($ntype in $ntypes) {
        foreach ($solution in $solutionFiles) {
            $solutionFilename = Split-Path $solution -Leaf
            $dataset = $solutionFilename -replace "-.*", ".csv"
            $datesetPath = Join-Path $dataPath $dataset
            $quadriplesList += [PSCustomObject]@{
                dataset  = $datesetPath
                improver = $improver
                ntype    = $ntype
                solution = $solution
            }
        }
    }
}

Write-Output "Processing $($quadriplesList.Count) solutions"

$quadriplesList | ForEach-Object -Parallel {
    $dataset = $_.dataset
    $improver = $_.improver
    $ntype = $_.ntype
    $solution = $_.solution    
    $solutionFilename = Split-Path $solution -Leaf
    $solutionFilenameNoExtension = $solutionFilename -replace ".txt", ""
    $solutionConfig = $solutionFilenameNoExtension.Substring(5)
    $datasetName = $solutionFilenameNoExtension.Substring(0, 4)
    $outFileName = "$datasetName-ls$improver$ntype-$solutionConfig.txt"
    $outFilePath = Join-Path $($using:solutionPath) $outFileName
    $ecpArgs = "-f ""$dataset"" -s ""$solution"" -o ""$outFilePath"" -t $improver -n $ntype"
    Write-Output "Processing $solutionFilename with $improver"
    $status = Start-Process $($using:program) -ArgumentList $ecpArgs -Wait -WindowStyle Hidden -PassThru
    if ($status.ExitCode -ne 0) {
        Write-Error "Error calling program with: $ecpArgs"
        Write-Error "Error processing $solution with $improver"
        Write-Error $status
    }
} -ThrottleLimit 48



