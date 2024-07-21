/*
*   Normie GPLGEN, A GUI GPL LICENSE/disclaimer generator (src\main2.c)
*   Copyright (C) 2024 Lilly H. St Claire
*
*            This program is free software: you can redistribute it and/or modify
*            it under the terms of the GNU General Public License as published by
*            the Free Software Foundation, either version 3 of the License, or (at
*            your option) any later version.
*
*            This program is distributed in the hope that it will be useful, but
*            WITHOUT ANY WARRANTY; without even the implied warranty of
*            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*            General Public License for more details.
*
*            You should have received a copy of the GNU General Public License
*            along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
*/
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commctrl.h>

#include "gpl.h"


enum CTL_IDS {
    ID_LICENSE_PATH_EDIT=100,
    ID_LICENSE_GENERATE_BUTTON=101,
    ID_PROJECT_PATH_EDIT=102,
    ID_PROJECT_NAME_EDIT=103,
    ID_YEAR_EDIT=104,
    ID_AUTHOR_EDIT=105,
    ID_FILE_GENERATE_BUTTON=106,
    ID_FILE_NAME_EDIT=107,
    ID_DESCRIPTION_EDIT=108,
    ID_COMMENT_STYLE_COMBO=109
};

typedef struct {
    HWND STATIC_title,
         STATIC_FileName,
         STATIC_ProjectName,
         STATIC_Year,
         STATIC_Author,
         STATIC_Description;
    HWND EDIT_ProjectPath,
         EDIT_LicensePath,
         EDIT_FileName,
         EDIT_ProjectName,
         EDIT_Year,
         EDIT_Author,
         EDIT_Description;
    HWND BUTTON_GenerateLicence,
         BUTTON_GenerateFile;
    HWND COMBO_CommentStyle;
} MAIN_GUI;

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lparam
);

MAIN_GUI create_MainGUI(HWND window_handle);

int WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        int       nCmdShow) {
    WNDCLASS window_class;
    HWND     window_handle;
    MAIN_GUI main_gui;
    MSG      msg;
    memset(&window_class, 0, sizeof(window_class));
    window_class.lpfnWndProc    = WindowProc;
    window_class.hInstance      = hInstance;
    window_class.lpszClassName  = TEXT("main window class");
    if(!RegisterClass(&window_class)) {
        printf("Error from RegisterClass: %d\n", GetLastError());
        exit(1);
    }
    window_handle = CreateWindowEx(
        0, TEXT("main window class"), TEXT("NORMIE GPLGEN"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        500, 500, NULL, NULL, hInstance, NULL
    );
    if(!window_handle) {
        printf("Error from CreateWindowEx: %d\n", GetLastError());
        exit(1);
    }
    ShowWindow(window_handle, nCmdShow);

    main_gui = create_MainGUI(window_handle);

    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK WindowProc(
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam) {
    static enum GPL_COMMENT_TYPE comment = CM_C;
    PAINTSTRUCT ps;
    HDC         hdc;
    switch(uMsg) {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1));
        EndPaint(hwnd, &ps);
        break;
    case WM_CLOSE:
        if(MessageBox(hwnd, TEXT("Really quit?"), TEXT("Alert"),
            MB_OKCANCEL) == IDOK) DestroyWindow(hwnd);
        break;
    case WM_COMMAND:
        if(LOWORD(wParam) == ID_COMMENT_STYLE_COMBO) {
            BOOL handled = FALSE;
            int i;
            LRESULT index;
            index = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            if(index < CM__MAX) comment = index;
            else MessageBox(hwnd, TEXT("Invalid Comment selection"), TEXT("User error!"), MB_OK);
        } else if(wParam == ID_LICENSE_GENERATE_BUTTON) {
            HWND handle;
            LPSTR proj, licence, path, err;
            int len;
            FILE *fp;
            handle = GetDlgItem(hwnd, ID_LICENSE_PATH_EDIT);
            len = GetWindowTextLength(handle)+1;
            if(!len) {
                MessageBox(hwnd, TEXT("Licence path is empty!"),
                    TEXT("User error!"), MB_OK);
                break;
            }
            licence = calloc(1,len);
            GetWindowText(handle, licence, len);
            handle = GetDlgItem(hwnd, ID_PROJECT_PATH_EDIT);
            len = GetWindowTextLength(handle)+1;
            if(!len) {
                MessageBox(hwnd, TEXT("Project path is empty!"),
                    TEXT("User error!"), MB_OK);
                free(licence);
                break;
            }
            proj = calloc(1,len);
            GetWindowText(handle, proj, len);
            path = malloc(strlen(licence)+strlen(proj)+2);
            sprintf(path, "%s\\%s", proj, licence);
            printf("Generating License at: %s\n", path);
            fp = fopen(path, "w");
            if(!fp) {
                err = malloc(strlen("fopen Error:  doesnt exist")+
                    strlen(path)+1);
                sprintf(err, "fopen Error: %s doesnt exist", path);
                MessageBox(hwnd, err, TEXT("User error!"), MB_OK);
                free(err);
                free(licence);
                free(proj);
                free(path);
                break;
            }
            fwrite(GPL_LICENCE_STRING, 1, strlen(GPL_LICENCE_STRING),
                fp);
            fclose(fp);
            free(path);
            free(licence);
            free(proj);
        } else if(wParam == ID_FILE_GENERATE_BUTTON) {
            HWND handle;
            LPSTR pn, au, yr, path, fname, ppath, desc, err, subname;
            int len;
            FILE *fp;
            handle = GetDlgItem(hwnd, ID_PROJECT_PATH_EDIT);
            len = GetWindowTextLength(handle)+1;
            if(!len) {
                MessageBox(hwnd, TEXT("Project path is empty!"),
                    TEXT("User error!"), MB_OK);
                break;
            }
            ppath = malloc(len);
            GetWindowText(handle, ppath, len);
            handle = GetDlgItem(hwnd, ID_PROJECT_NAME_EDIT);
            len = GetWindowTextLength(handle)+1;
            if(!len) {
                MessageBox(hwnd, TEXT("Project name is empty!"),
                    TEXT("User error!"), MB_OK);
                free(ppath);
                break;
            }
            pn = malloc(len);
            GetWindowText(handle, pn, len);
            handle = GetDlgItem(hwnd, ID_AUTHOR_EDIT);
            len = GetWindowTextLength(handle)+1;
            if(!len) {
                MessageBox(hwnd, TEXT("Author is empty!"),
                    TEXT("User error!"), MB_OK);
                free(ppath);
                free(pn);
                break;
            }
            au = malloc(len);
            GetWindowText(handle, au, len);
            handle = GetDlgItem(hwnd, ID_YEAR_EDIT);
            len = GetWindowTextLength(handle)+1;
            if(!len) {
                MessageBox(hwnd, TEXT("Year is empty!"),
                    TEXT("User error!"), MB_OK);
                free(ppath);
                free(pn);
                free(au);
                break;
            }
            yr = malloc(len);
            GetWindowText(handle, yr, len);
            handle = GetDlgItem(hwnd, ID_DESCRIPTION_EDIT);
            len = GetWindowTextLength(handle)+1;
            if(!len) {
                MessageBox(hwnd, TEXT("Description is empty!"),
                    TEXT("User error!"), MB_OK);
                free(ppath);
                free(yr);
                free(pn);
                free(au);
                break;
            }
            desc = malloc(len);
            GetWindowText(handle, desc, len);
            handle = GetDlgItem(hwnd, ID_FILE_NAME_EDIT);
            len = GetWindowTextLength(handle)+1;
            if(!len) {
                MessageBox(hwnd, TEXT("File name is empty!"),
                    TEXT("User error!"), MB_OK);
                free(ppath);
                free(pn);
                free(au);
                free(yr);
                free(desc);
                break;
            }
            fname = malloc(len);
            GetWindowText(handle, fname, len);
            path = malloc(strlen(fname)+strlen(ppath)+2);
            sprintf(path, "%s\\%s", ppath, fname);
            fp = fopen(path, "w");
            if(!fp) {
                err = malloc(strlen("fopen Error:  doesnt exist!")+strlen(path));
                sprintf(err, "fopen Error: %s doesnt exist!", path);
                MessageBox(hwnd, err, TEXT("User error!"), MB_OK);
                free(err);
                free(ppath);
                free(fname);
                free(pn);
                free(au);
                free(yr);
                free(path);
            }
            subname = malloc(strlen(fname)+3);
            sprintf(subname, "(%s)", fname);
            GPLGENFP(fp, comment, pn, desc, subname, yr, au);
            free(subname);
            fclose(fp);
            free(ppath);
            free(fname);
            free(pn);
            free(au);
            free(yr);
            free(path);
        } break;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

#define CREATE_STATIC(content, x, y, w, h, l, o)\
    CreateWindowEx(0, TEXT("STATIC"), content, WS_CHILD | WS_VISIBLE,\
        x+o*textmetric.tmMaxCharWidth,y+l*textmetric.tmHeight,\
        w*textmetric.tmMaxCharWidth,\
        h*textmetric.tmHeight,\
        window_handle, 0,\
        (HINSTANCE)GetWindowLongPtr(window_handle, GWLP_HINSTANCE),\
        NULL)
#define CREATE_EDIT(content, x, y, w, h, l, o, id)\
    CreateWindowEx(0, TEXT("EDIT"), content, WS_CHILD | WS_VISIBLE|\
        WS_BORDER|ES_LEFT|ES_AUTOHSCROLL,\
        x+o*textmetric.tmMaxCharWidth,y+l*textmetric.tmHeight,\
        w*textmetric.tmMaxCharWidth,\
        h*textmetric.tmHeight,\
        window_handle, (HMENU)id,\
        (HINSTANCE)GetWindowLongPtr(window_handle, GWLP_HINSTANCE),\
        NULL)

MAIN_GUI create_MainGUI(HWND window_handle) {
    MAIN_GUI    ret;
    HDC         hdc;
    TEXTMETRIC  textmetric;
    int         MapMode;

    hdc = GetDC(window_handle);
    MapMode = GetMapMode(hdc);
    if(MapMode != MM_TEXT)
        SetMapMode(hdc, MM_TEXT);
    GetTextMetrics(hdc, &textmetric);
    SetMapMode(hdc, MapMode);
    
    ret.STATIC_title =
        CREATE_STATIC("GPLGEN for normies", 5, 0, 10, 1, 0, 0);
    
    ret.STATIC_FileName =
        CREATE_STATIC("File name:", 5, 40, 10, 1, 2, 0);
    ret.EDIT_FileName =
        CREATE_EDIT(TEXT("..."), 20, 40, 10, 1, 2, 10,
            ID_FILE_NAME_EDIT);
    ret.STATIC_ProjectName =
        CREATE_STATIC("Project name:", 5, 40, 10, 1, 3, 0);
    ret.EDIT_ProjectName =
        CREATE_EDIT(TEXT("..."), 20, 40, 10, 1, 3, 10,
            ID_PROJECT_NAME_EDIT);
    ret.STATIC_Year =
        CREATE_STATIC("Copyright year:", 5, 40, 10, 1, 4, 0);
    ret.EDIT_Year =
        CREATE_EDIT(TEXT("..."), 20, 40, 10, 1, 4, 10,
            ID_YEAR_EDIT);
    ret.STATIC_Author =
        CREATE_STATIC("Author:", 5, 40, 10, 1, 5, 0);
    ret.EDIT_Author =
        CREATE_EDIT(TEXT("..."), 20, 40, 10, 1, 5, 10,
            ID_AUTHOR_EDIT);
    ret.STATIC_Description =
        CREATE_STATIC("Description:", 5, 40, 10, 1, 6, 0);
    ret.EDIT_Description =
        CREATE_EDIT(TEXT("..."), 20, 40, 10, 1, 6, 10,
            ID_DESCRIPTION_EDIT);

    TCHAR comment_styles[CM__MAX][8]= {
        TEXT("C"),
        TEXT("C++"),
        TEXT("Python"),
        TEXT("VBasic"),
        TEXT("Ocaml")
    };


    ret.COMBO_CommentStyle = CreateWindow(WC_COMBOBOX, TEXT(""),
        CBS_DROPDOWN|CBS_HASSTRINGS|WS_CHILD|WS_OVERLAPPED|WS_VISIBLE,
        20+20*textmetric.tmMaxCharWidth, 40+6*textmetric.tmHeight,
            10*textmetric.tmMaxCharWidth, 10*textmetric.tmHeight,
            window_handle, (HMENU)ID_COMMENT_STYLE_COMBO,
            (HINSTANCE)GetWindowLongPtr(window_handle, GWLP_HINSTANCE),
            NULL
        );
    TCHAR A[16];
    int k = 0;
    memset(&A, 0, sizeof(A));
    for(k=0; k<CM__MAX; k++) {
        memcpy(A, (TCHAR*)comment_styles[k], sizeof(A)/sizeof(TCHAR));
        SendMessage(ret.COMBO_CommentStyle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
    }
    SendMessage(ret.COMBO_CommentStyle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

    ret.EDIT_ProjectPath =
        CREATE_EDIT(TEXT("Project path here..."), 10, 20, 10, 1, 0, 0,
            ID_PROJECT_PATH_EDIT);
    ret.EDIT_LicensePath =
        CREATE_EDIT(TEXT("License path here..."), 20, 30, 10, 1, 1, 10,
            ID_LICENSE_PATH_EDIT);
    ret.BUTTON_GenerateLicence = CreateWindowEx(
        0, TEXT("BUTTON"),
        TEXT("Generate LICENSE"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, textmetric.tmHeight+25,
            10*textmetric.tmMaxCharWidth, textmetric.tmHeight+10,
        window_handle,
        (HMENU)(ID_LICENSE_GENERATE_BUTTON),
        (HINSTANCE)GetWindowLongPtr(window_handle, GWLP_HINSTANCE),
        NULL);
    ret.BUTTON_GenerateFile = CreateWindowEx(
        0, TEXT("BUTTON"),
        TEXT("Generate File"),
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        10, 10*textmetric.tmHeight+25,
            10*textmetric.tmMaxCharWidth, textmetric.tmHeight+10,
        window_handle,
        (HMENU)(ID_FILE_GENERATE_BUTTON),
        (HINSTANCE)GetWindowLongPtr(window_handle, GWLP_HINSTANCE),
        NULL);

    return ret;   
}
