var
  Search: WideString;
  SearchPos: Cardinal;
begin
  Search := '\TODO';
  SearchPos := WideLength(FilePath) - WideLength(Search) + 1;
  if WideTextPosEx(Search,FilePath,SearchPos) = SearchPos then
  begin
  FileName := 'DONE';
  end;
end.
