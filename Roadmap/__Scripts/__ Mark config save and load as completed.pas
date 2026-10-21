var
  Search: WideString;
  SearchPos: Cardinal;
begin
  Search := 'Config.h (structures)\TODO';
  SearchPos := WideLength(FilePath) - WideLength(Search) + 1;
  if WideTextPosEx(Search,FilePath,SearchPos) = SearchPos then
  begin
  FileName := 'DONE';
  end;
  Search := 'Config.cpp (load & store)\TODO';
  SearchPos := WideLength(FilePath) - WideLength(Search) + 1;
  if WideTextPosEx(Search,FilePath,SearchPos) = SearchPos then
  begin
  FileName := 'DONE';
  end;
end.
