' Description: ��gb2312�ļ�ת����utf-8�ļ�
' Version: 1.0
' Autor: 
' Date: 2022-04-04 22:18:06
' LastEditors: 
' LastEditTime: 2022-04-04 22:38:56
 
Set objArgs = WScript.Arguments   
If objArgs.Count>0 Then  
    If MsgBox("�������֧��GB2312��UTF-8��ת��"&vbCrLf&"��������϶����ļ����б�ĸ�ʽ���ļ���㡰ȡ����", vbOKCancel + vbExclamation + vbDefaultButton2, "֣������!!!") = vbOK Then  
        For I = 0 To objArgs.Count - 1   
            FileUrl = objArgs(I)   
            Call WriteToFile(FileUrl, ReadFile(FileUrl, "gb2312"), "utf-8")   
        Next  
    End If  
Else  
    MsgBox "�뽫��Ҫת���ġ�gb2312�ļ����ϵ�����ļ���"&vbCrLf&"����Ὣ�����Զ�ת��Ϊutf-8�ļ�", vbInformation, "��ܰ��ʾ��"  
End If  
  
'-------------------------------------------------   
'��������:ReadFile   
'����:����AdoDb.Stream��������ȡ���ָ�ʽ���ı��ļ�   
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
'��������:WriteToFile   
'����:����AdoDb.Stream������д����ָ�ʽ���ı��ļ�   
'����:FileUrl-�ļ����·��;Str-�ļ�����;CharSet-�����ʽ(utf-8,gb2312.....)   
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