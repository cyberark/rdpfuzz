$FUZZ_ROOT = "C:\RDPFuzz"

$CRASH_PATH      = "$FUZZ_ROOT\Logs\dumps"
$CSV_PATH        = "$FUZZ_ROOT\Logs\dumps\crashes.csv"
$WINAFL_LOG_PATH = "$FUZZ_ROOT\Logs"
$ANALYZED_PATH   = "$FUZZ_ROOT\Logs\dumps\analyzed"
$CDB_PATH        = "C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe"
$SCRIPT_PATH     = "$FUZZ_ROOT\bin\scripts\analyze_crash_cdb.js"
$ANALYZE_COMMAND = ". '$CDB_PATH' /z {0} /c '.scriptload $SCRIPT_PATH; !analyzeCrash {1};q'"
$EXCEPTIONS      = @{}
$CSV_DATA        = @()

# send emails
function Send-Mail
{
    param ($subject, $body)

    $email = "XXXMONITOR_EMAILXXX@gmail.com" 
    $pass = "MONITOR_EMAIL_PASS" 
    $smtpServer = "smtp.gmail.com" 
    $msg = new-object Net.Mail.MailMessage 
    $smtp = new-object Net.Mail.SmtpClient($smtpServer) 
    $smtp.UseDefaultCredentials = $true;
    $smtp.EnableSsl = $true 
    $msg.From = "$email"  
    $msg.To.Add("XXXYOUR_MAIL_HEREXXX@gmail.com") 
    $msg.BodyEncoding = [system.Text.Encoding]::Unicode 
    $msg.SubjectEncoding = [system.Text.Encoding]::Unicode 
    $msg.IsBodyHTML = $true  
    $msg.Subject = $subject
    $msg.Body = $body
    $SMTP.Credentials = New-Object System.Net.NetworkCredential("$email", "$pass"); 
    $smtp.Send($msg)
}

# Get all exception records
function Get-ExceptionRecord{
	param ($crash_pid, $winafl_log_content)

    try
    {
        $crash_lines = $winafl_log_content | Select-String -Context 0,1 pid=$crash_pid
	    $crash_except = '0x' + $crash_lines[-1][-1].tostring().split('=')[-2].split(' ')[0]
    }
    catch
    {
        Write-Host -ForegroundColor Red "ERROR $($crash_pid)"
        return -1
    }
	return $crash_except
}


# Analyze dumps
function Analyze-Dumps {
	$winafl_log_content = gc $WINAFL_LOG_PATH\winafl-crash-log.txt

    Write-Verbose "[+] Analyzing crash dumps"
    $CSV_DATA        = @()
    foreach ($dump in (Get-ChildItem $CRASH_PATH\*.dmp | ? {$_.LastWriteTime -lt (Get-Date).AddMinutes(-5)})) {
        Write-Verbose "`tAnalyzing $($dump.Name)"
        $crash_pid = $dump[0].name.split('.').split('_')[0]

		$exception_record = Get-ExceptionRecord $crash_pid $winafl_log_content

        if ($exception_record -eq -1)
        {
            continue
        }

        $output = & Invoke-Expression ($ANALYZE_COMMAND -f $dump.FullName, $exception_record)
        $output = $output[($output.IndexOf('-'*32)+1)..$output.Length]
        $exception_data = $output[0..($output.IndexOf('-'*32)-1)]
        $output = $output[($output.IndexOf('-'*32)+1)..$output.Length]
        $stack = $output[0..($output.IndexOf('-'*32)-1)]

        $stack_general = $stack[5..$stack.Length] | ForEach-Object {$_.split(' ')[-1]}

        $CSV_DATA += [PSCustomObject]@{
            pid   = $crash_pid
            stack = $stack_general | Out-String
            data  = $exception_data + $stack | Out-String
			file  = $dump
        }
    }
    return $CSV_DATA
}


while ($true)
{
    Write-Host "starting"

    $database = Import-Csv $CSV_PATH 
    $unique_exceptions = $database | select stack -Unique
    $unique_stacks = @()
    $unique_exceptions | %{ $unique_stacks+= $_.stack }

    $CSV_DATA = Analyze-Dumps # remove the copying of the dumps outside this func

    # Check if the crashes are new
    foreach ($e in $CSV_DATA) 
    {
        if ($e.stack -in $unique_stacks)
        {
            Remove-Item $e.file.FullName
        }
        else
        {
            $unique_stacks += $e.stack

            # save it
		    $copy_name = $e.file.Name.Split('.')
            $copy_name[0] += $(get-date -f hhmmssff-dd-MM-yy)
            Move-Item $e.file.FullName "$ANALYZED_PATH\$($copy_name -join '.')"

            Write-Host -ForegroundColor Magenta "NEW CRASH :)"
            Write-Host $e.stack

            # let us know :)
            Send-Mail "NEW CRASH in $($Env:COMPUTERNAME) :)" "$($e.stack)"
        }
    }

    # Write results to CSV
    Write-Verbose "[+] Writing results to $CSV_PATH"
    $CSV_DATA | Select-Object -Property pid, stack, data | Export-Csv -Path $CSV_PATH -Append

    Write-Host -ForegroundColor Green "[+] Done"

    sleep -Seconds 600
}