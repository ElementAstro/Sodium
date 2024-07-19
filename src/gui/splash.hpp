#include <wx/button.h>  // 添加 wx/button.h 头文件
#include <wx/colour.h>
#include <wx/graphics.h>
#include <wx/sizer.h>  // 添加 wx/sizer.h 头文件
#include <wx/splash.h>
#include <wx/wx.h>

class MySplashScreen : public wxFrame {
public:
    MySplashScreen(const wxString& bitmapPath, const wxString& label,
                   int timeout);

private:
    wxBoxSizer* m_sizer;

    void OnTimer(wxTimerEvent& event);
};

MySplashScreen::MySplashScreen(const wxString& bitmapPath,
                               const wxString& label, int timeout)
    : wxFrame(nullptr, wxID_ANY, "Splash Screen", wxDefaultPosition,
              wxDefaultSize, wxBORDER_NONE | wxFRAME_SHAPED),
      m_sizer(new wxBoxSizer(wxVERTICAL)) {
    // 添加位图
    wxBitmap bitmap;
    if (bitmap.LoadFile(bitmapPath)) {  // 判断图片是否加载成功
        auto bitmapCtrl =
            new wxStaticBitmap(this, wxID_ANY, bitmap, wxDefaultPosition,
                               wxDefaultSize, wxBORDER_NONE);
        m_sizer->Add(bitmapCtrl, wxSizerFlags().Expand().Proportion(1));
    } else {
        wxMessageBox("Splash Image Not Found", "Warning",
                     wxICON_WARNING | wxOK);
    }

    // 添加标签和计时器
    auto timer = new wxTimer(this, wxID_ANY);
    Bind(wxEVT_TIMER, &MySplashScreen::OnTimer, this, wxID_ANY);

    auto labelCtrl = new wxStaticText(this, wxID_ANY, label);
    m_sizer->Add(labelCtrl, wxSizerFlags().Center().Border(wxALL, 10));

    // 添加关闭按钮
    auto button = new wxButton(this, wxID_OK, "&关闭");
    m_sizer->Add(button, wxSizerFlags().Center().Border(wxALL, 10));

    // 设置布局管理器
    SetSizer(m_sizer);
    Fit();

    // 启动定时器
    if (timeout > 0) {
        timer->StartOnce(timeout * 1000);
    }
}

void MySplashScreen::OnTimer(wxTimerEvent& event) {
    Close(true);
    Destroy();
}
