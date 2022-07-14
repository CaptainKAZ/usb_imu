' Description: 将gb2312文件转换成utf-8文件
' Version: 1.0
' Autor: 
' Date: 2022-04-04 22:18:06
' LastEditors: 
' LastEditTime: 2022-04-04 22:38:56
 
Set objArgs = WScript.Arguments   
If objArgs.Count>0 Then  
    If MsgBox("本程序仅支持GB2312到UTF-8的转换"&vbCrLf&"如果您所拖动的文件里有别的格式的文件请点“取消”", vbOKCancel + vbExclamation + vbDefaultButton2, "郑重提醒!!!") = vbOK Then  
        For I = 0 To objArgs.Count - 1   
            FileUrl = objArgs(I)   
            Call WriteToFile(FileUrl, ReadFile(FileUrl, "gb2312"), "utf-8")   
        Next  
    End If  
Else  
    MsgBox "请将您要转换的“gb2312文件”拖到这个文件上"&vbCrLf&"程序会将它们自动转换为utf-8文件", vbInformation, "温馨提示："  
End If  
  
'-------------------------------------------------   
'函数名称:ReadFile   
'作用:利用AdoDb.Stream对象来读取各种格式的文本文件   
'----------------------------------------------------   
  
Function ReadFile(FileUrl, CharSet)   
    Dim Str   
    Set stm = CreateObject("Adodb.Stream")   
    stm.Type = 2   
    stm.mode = 3   
    stm.charset = CharSet  
    stm.Open  
    stm.loadfromfile FileUrl   
    Str = stm.readtext   
    stm.Close  
    Set stm = Nothing  
    ReadFile = Str   
End Function  
  
'-------------------------------------------------   
'函数名称:WriteToFile   
'作用:利用AdoDb.Stream对象来写入各种格式的文本文件   
'参数:FileUrl-文件相对路径;Str-文件内容;CharSet-编码格式(utf-8,gb2312.....)   
'----------------------------------------------------   
  
Function WriteToFile (FileUrl, Str, CharSet)   
    Set stm = CreateObject("Adodb.Stream")   
    stm.Type = 2   
    stm.mode = 3   
    stm.charset = CharSet  
    stm.Open  
    stm.WriteText Str   
    stm.SaveToFile FileUrl, 2   
    stm.flush  
    stm.Close  
    Set stm = Nothing  
End Function