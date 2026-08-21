Set fso = CreateObject("Scripting.FileSystemObject")

' --- 1. AUTOMATIC FILE SEARCH IN DEBUG FOLDER ---
lkfFile = ""
mapFile = ""

If fso.FolderExists("Debug") Then
    Set debugFolder = fso.GetFolder("Debug")
    For Each f In debugFolder.Files
        If LCase(fso.GetExtensionName(f.Name)) = "lkf" Then
            lkfFile = f.Path
        ElseIf LCase(fso.GetExtensionName(f.Name)) = "map" Then
            mapFile = f.Path
        End If
    Next
End If

If lkfFile = "" Or mapFile = "" Then
    WScript.Echo "Error: .lkf or .map file not found in Debug folder!"
    WScript.Quit
End If

' --- 2. READING FILES ---
Set file = fso.OpenTextFile(lkfFile, 1)
lkfText = file.ReadAll
file.Close

Set file = fso.OpenTextFile(mapFile, 1)
mapText = file.ReadAll
file.Close

' --- 3. PARSING FUNCTIONS ---
Function GetMapSize(patternStr)
    Set regEx = New RegExp
    regEx.Pattern = "length\s+(\d+)\s+segment\s+" & patternStr
    regEx.IgnoreCase = True
    Set matches = regEx.Execute(mapText)
    If matches.Count > 0 Then GetMapSize = Clng(matches(0).SubMatches(0)) Else GetMapSize = 0
End Function

Function GetLkfLimit(segName)
    Set regEx = New RegExp
    regEx.Pattern = "seg\s+" & segName & ".*-m\s+0x([0-9a-fA-F]+)"
    regEx.IgnoreCase = True
    Set matches = regEx.Execute(lkfText)
    If matches.Count > 0 Then GetLkfLimit = Clng("&H" & matches(0).SubMatches(0)) Else GetLkfLimit = 0
End Function

Function GetLkfVectorSize()
    Set regEx = New RegExp
    regEx.Pattern = "seg\s+\.const\s+-b\s+0x[0-9a-fA-F]+\s+-k"
    regEx.IgnoreCase = True
    If regEx.Test(lkfText) Then GetLkfVectorSize = 128 Else GetLkfVectorSize = 0
End Function

Function GetLkfDefine(varName)
    Set regEx = New RegExp
    regEx.Pattern = "def\s+" & varName & "=0x([0-9a-fA-F]+)"
    regEx.IgnoreCase = True
    Set matches = regEx.Execute(lkfText)
    If matches.Count > 0 Then GetLkfDefine = Clng("&H" & matches(0).SubMatches(0)) Else GetLkfDefine = 0
End Function

' --- 4. COLLECTING DATA FROM MAP & LKF ---
text_size       = GetMapSize("\.text")
const_size      = GetMapSize("\.const")
init_size       = GetMapSize("\.init")
bsct_from       = GetMapSize("\.bsct,\s+from")
bsct_ram        = GetMapSize("\.bsct,\s+initialized")
ubsct           = GetMapSize("\.ubsct")
bit_size        = GetMapSize("\.bit")
share           = GetMapSize("\.share")
data_size       = GetMapSize("\.data")
bss             = GetMapSize("\.bss")

' Dynamic limit parsing from LKF
limit_const_flash = GetLkfLimit("\.const")
limit_zero_page   = GetLkfLimit("\.bsct")
limit_near_ram    = GetLkfLimit("\.data")
vector_size       = GetLkfVectorSize()

' Dynamic stack limits from LKF configuration variables
def_endmem  = GetLkfDefine("__endmem")
def_stack   = GetLkfDefine("__stack")

' Dynamic Stack size from MAP
Set regEx = New RegExp
regEx.Pattern = "Stack size:\s*(\d+)"
Set matches = regEx.Execute(mapText)
stack_size = 0
If matches.Count > 0 Then stack_size = Clng(matches(0).SubMatches(0))

' --- 5. DYNAMIC TOTALS CALCULATIONS ---
flash_used  = text_size + const_size + init_size + bsct_from
flash_total = limit_const_flash + vector_size
flash_free  = flash_total - flash_used

zpage_used  = bsct_ram + ubsct + bit_size + share
near_used   = data_size + bss

' Stack allocated limit = Top of RAM address - End of Near RAM variables address
limit_stack = def_stack - def_endmem 
ram_used    = zpage_used + near_used + stack_size
ram_total   = def_stack + 1
ram_free    = ram_total - ram_used

' --- 6. OUTPUT REPORT (NO HARDCODE, LATIN ONLY) ---
WScript.Echo "--------------------------------------------------"
WScript.Echo "          MEMORY REPORT"
WScript.Echo "--------------------------------------------------"
WScript.Echo "FLASH MEMORY STATUS:"
WScript.Echo "  .text   (Code size)         : " & text_size & " b"
WScript.Echo "  .const  (Strings/Constants) : " & const_size & " b"
WScript.Echo "  .init   (Startup init)      : " & init_size & " b"
WScript.Echo "  .bsct_f (RAM copies)        : " & bsct_from & " b"
WScript.Echo "  [TOTAL FLASH USED           : " & flash_used & " / " & flash_total & " b]"
WScript.Echo ""
WScript.Echo "ZERO PAGE RAM STATUS:"
WScript.Echo "  .bsct   (Initialized vars)  : " & bsct_ram & " b"
WScript.Echo "  .ubsct  (Zero vars)         : " & ubsct & " b"
WScript.Echo "  .bit/.share                 : " & (bit_size + share) & " b"
WScript.Echo "  [TOTAL ZERO PAGE USED       : " & zpage_used & " / " & limit_zero_page & " b]"
WScript.Echo ""
WScript.Echo "NEAR RAM STATUS:"
WScript.Echo "  .data   (Init near vars)    : " & data_size & " b"
WScript.Echo "  .bss    (Graph arrays)      : " & bss & " b"
WScript.Echo "  [TOTAL NEAR RAM USED        : " & near_used & " / " & limit_near_ram & " b]"
WScript.Echo ""
WScript.Echo "STACK MEMORY STATUS:"
WScript.Echo "  Stack   (Calls/Interrupts)  : " & stack_size & " / " & limit_stack & " b"
WScript.Echo ""
WScript.Echo "--------------------------------------------------"
WScript.Echo "FINAL CHIP MEMORY STATUS:"
WScript.Echo "--------------------------------------------------"
WScript.Echo "	FLASH : Total " & flash_total & " b | Used " & flash_used & " b | Free " & flash_free & " b"
WScript.Echo "	RAM   : Total " & ram_total & " b | Used " & ram_used & " b | Free " & ram_free & " b"
WScript.Echo "--------------------------------------------------"
