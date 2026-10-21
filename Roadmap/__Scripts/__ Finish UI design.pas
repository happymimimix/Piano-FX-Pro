var
  Search: WideString;
  SearchPos: Cardinal;
begin
  Search := 'PianoFromAbove.rc (ui design)\TODO';
  SearchPos := WideLength(FilePath) - WideLength(Search) + 1;
  if WideTextPosEx(Search,FilePath,SearchPos) = SearchPos then
  begin
  FileName := 'DONE';
  end;
end.
