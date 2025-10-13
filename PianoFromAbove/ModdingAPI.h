#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <string>
#include <vector>
#include <regex>
using namespace std;

struct srcfile_t {
    wstring FilePath = L"";
    wstring Content = L"";
    void Load() {
        wifstream file(this->FilePath, ios::binary); //Do not make the unnecessary CRLF to LF and LF to CRLF conversion!!! 
        file.imbue(locale("en_US.UTF-8"));
        wstringstream buffer;
        buffer << file.rdbuf();
        this->Content = buffer.str();
        file.close();
    }
    void Save() {
        wofstream file(this->FilePath, ios::binary); //Do not make the unnecessary CRLF to LF and LF to CRLF conversion!!! 
        file.imbue(locale("en_US.UTF-8"));
        file << this->Content;
        file.close();
    }
};

struct cppsrc_t {
    shared_ptr<srcfile_t> Document;
    size_t Index = 0; //Defines selection range
    size_t Length = 0;
    shared_ptr<cppsrc_t> Parent = nullptr;
    bool IsRoot() { return Parent == nullptr; }
    shared_ptr<cppsrc_t> FindRoot() {
        shared_ptr<cppsrc_t> Current = this->Parent;
        while (!Current->IsRoot()) {
            Current = Current->Parent;
        }
        return Current;
    }
    size_t CountParents() {
        size_t Count = 0;
        shared_ptr<cppsrc_t> Current = this->Parent;
        while (!Current->IsRoot()) {
            Current = Current->Parent;
            Count++;
        }
        return Count;
    }
    wstring GetText() {
        return this->Document->Content.substr(this->Index, this->Length);
    }
    void Replace(const wstring& NewText) {
        this->Document->Content.replace(this->Index, this->Length, NewText);
    }
    void InsertBefore(const wstring& text) {
        this->Document->Content.insert(this->Index, text);
        this->Index += text.length();
    }
    void InsertAfter(const wstring& text) {
        this->Document->Content.insert(this->Index + this->Length, text);
    }
    void Delete() {
        this->Document->Content.erase(this->Index, this->Length);
        this->Length = 0;
    }
    void Close() {
        this->Document->Save();
    }
};

cppsrc_t RegExprSearch(cppsrc_t Source, const wstring& pattern, long long index) {
    if (index == 0) {
        throw invalid_argument("Index cannot be zero");
    }

    wstring text = Source.GetText();
    wregex regex(pattern);
    vector<wsmatch> matches;

    // Find all
    for (wsregex_iterator it(text.begin(), text.end(), regex); it != wsregex_iterator(); it++) {
        matches.push_back(*it);
    }

    if (matches.empty()) {
        throw runtime_error("Pattern not found");
    }

    size_t matchidx;
    if (index < 0) {
        if (-index > matches.size()) {
            throw out_of_range("Index exceeds number of matches");
        }
        matchidx = matches.size() + index;
    }
    else {
        if (index > matches.size()) {
            throw out_of_range("Index exceeds number of matches");
        }
        matchidx = index - 1;
    }

    const wsmatch& match = matches[matchidx];

    // Create result object
    cppsrc_t result;
    result.Document = Source.Document;
    result.Index = Source.Index + match.position();
    result.Length = match.length();
    result.Parent = make_shared<cppsrc_t>(Source);

    return result;
}

cppsrc_t LeftStrip(cppsrc_t Source, const wstring& pattern) {
    cppsrc_t match = RegExprSearch(Source, pattern, 1);

    // Create result object
    cppsrc_t result;
    result.Document = Source.Document;
    result.Index = match.Index + match.Length;
    result.Length = Source.Index + Source.Length - result.Index;
    result.Parent = make_shared<cppsrc_t>(Source);

    return result;
}

cppsrc_t RightStrip(cppsrc_t Source, const wstring& pattern) {
    cppsrc_t match = RegExprSearch(Source, pattern, -1);

    // Create result object
    cppsrc_t result;
    result.Document = Source.Document;
    result.Index = Source.Index;
    result.Length = match.Index - Source.Index;
    result.Parent = make_shared<cppsrc_t>(Source);

    return result;
}

static char BraceOpposite(char ch) noexcept {
    switch (ch) {
    case '(':
        return ')';
    case ')':
        return '(';
    case '[':
        return ']';
    case ']':
        return '[';
    case '{':
        return '}';
    case '}':
        return '{';
    case '<':
        return '>';
    case '>':
        return '<';
    default:
        return '\0';
    }
}

size_t BraceMatch(const wstring& text, size_t index) {
    const wchar_t chBrace = text[index];
    const wchar_t chSeek = BraceOpposite(chBrace);
    if (chSeek == L'\0')
        return wstring::npos;

    // Determine search direction
    int direction = -1;
    if (chBrace == L'(' || chBrace == L'[' || chBrace == L'{' || chBrace == L'<')
        direction = 1;

    int depth = 1;
    size_t pos = index;

    while (true) {
        if (direction > 0) {
            pos++;
            if (pos >= text.size()) break;
        }
        else {
            if (pos <= 0) break;
            pos--;
        }

        wchar_t chAt = text[pos];
        if (chAt == chBrace)
            ++depth;
        else if (chAt == chSeek)
            --depth;

        if (depth == 0)
            return pos;
    }

    return wstring::npos;
}

cppsrc_t OpenCppFile(wstring Path) {
    srcfile_t SourceFile = {};
    SourceFile.FilePath = Path;
    SourceFile.Load();
    cppsrc_t RootPointer = {};
    RootPointer.Document = make_shared<srcfile_t>(SourceFile);
    RootPointer.Length = SourceFile.Content.length();
    return RootPointer;
}
