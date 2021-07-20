// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=0fb3a032a91b41bf31ee3b714fda4f0db881d6ad$
//

#ifndef CEF_LIBCEF_DLL_CTOCPP_VIEWS_MENU_BUTTON_CTOCPP_H_
#define CEF_LIBCEF_DLL_CTOCPP_VIEWS_MENU_BUTTON_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_CEF_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/views/cef_menu_button_capi.h"
#include "include/views/cef_menu_button.h"
#include "libcef_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class CefMenuButtonCToCpp : public CefCToCppRefCounted<CefMenuButtonCToCpp,
                                                       CefMenuButton,
                                                       cef_menu_button_t> {
 public:
  CefMenuButtonCToCpp();

  // CefMenuButton methods.
  void ShowMenu(CefRefPtr<CefMenuModel> menu_model,
                const CefPoint& screen_point,
                cef_menu_anchor_position_t anchor_position) OVERRIDE;
  void TriggerMenu() OVERRIDE;

  // CefLabelButton methods.
  CefRefPtr<CefMenuButton> AsMenuButton() OVERRIDE;
  void SetText(const CefString& text) OVERRIDE;
  CefString GetText() OVERRIDE;
  void SetImage(cef_button_state_t button_state,
                CefRefPtr<CefImage> image) OVERRIDE;
  CefRefPtr<CefImage> GetImage(cef_button_state_t button_state) OVERRIDE;
  void SetTextColor(cef_button_state_t for_state, cef_color_t color) OVERRIDE;
  void SetEnabledTextColors(cef_color_t color) OVERRIDE;
  void SetFontList(const CefString& font_list) OVERRIDE;
  void SetHorizontalAlignment(cef_horizontal_alignment_t alignment) OVERRIDE;
  void SetMinimumSize(const CefSize& size) OVERRIDE;
  void SetMaximumSize(const CefSize& size) OVERRIDE;

  // CefButton methods.
  CefRefPtr<CefLabelButton> AsLabelButton() OVERRIDE;
  void SetState(cef_button_state_t state) OVERRIDE;
  cef_button_state_t GetState() OVERRIDE;
  void SetInkDropEnabled(bool enabled) OVERRIDE;
  void SetTooltipText(const CefString& tooltip_text) OVERRIDE;
  void SetAccessibleName(const CefString& name) OVERRIDE;

  // CefView methods.
  CefRefPtr<CefBrowserView> AsBrowserView() OVERRIDE;
  CefRefPtr<CefButton> AsButton() OVERRIDE;
  CefRefPtr<CefPanel> AsPanel() OVERRIDE;
  CefRefPtr<CefScrollView> AsScrollView() OVERRIDE;
  CefRefPtr<CefTextfield> AsTextfield() OVERRIDE;
  CefString GetTypeString() OVERRIDE;
  CefString ToString(bool include_children) OVERRIDE;
  bool IsValid() OVERRIDE;
  bool IsAttached() OVERRIDE;
  bool IsSame(CefRefPtr<CefView> that) OVERRIDE;
  CefRefPtr<CefViewDelegate> GetDelegate() OVERRIDE;
  CefRefPtr<CefWindow> GetWindow() OVERRIDE;
  int GetID() OVERRIDE;
  void SetID(int id) OVERRIDE;
  int GetGroupID() OVERRIDE;
  void SetGroupID(int group_id) OVERRIDE;
  CefRefPtr<CefView> GetParentView() OVERRIDE;
  CefRefPtr<CefView> GetViewForID(int id) OVERRIDE;
  void SetBounds(const CefRect& bounds) OVERRIDE;
  CefRect GetBounds() OVERRIDE;
  CefRect GetBoundsInScreen() OVERRIDE;
  void SetSize(const CefSize& size) OVERRIDE;
  CefSize GetSize() OVERRIDE;
  void SetPosition(const CefPoint& position) OVERRIDE;
  CefPoint GetPosition() OVERRIDE;
  CefSize GetPreferredSize() OVERRIDE;
  void SizeToPreferredSize() OVERRIDE;
  CefSize GetMinimumSize() OVERRIDE;
  CefSize GetMaximumSize() OVERRIDE;
  int GetHeightForWidth(int width) OVERRIDE;
  void InvalidateLayout() OVERRIDE;
  void SetVisible(bool visible) OVERRIDE;
  bool IsVisible() OVERRIDE;
  bool IsDrawn() OVERRIDE;
  void SetEnabled(bool enabled) OVERRIDE;
  bool IsEnabled() OVERRIDE;
  void SetFocusable(bool focusable) OVERRIDE;
  bool IsFocusable() OVERRIDE;
  bool IsAccessibilityFocusable() OVERRIDE;
  void RequestFocus() OVERRIDE;
  void SetBackgroundColor(cef_color_t color) OVERRIDE;
  cef_color_t GetBackgroundColor() OVERRIDE;
  bool ConvertPointToScreen(CefPoint& point) OVERRIDE;
  bool ConvertPointFromScreen(CefPoint& point) OVERRIDE;
  bool ConvertPointToWindow(CefPoint& point) OVERRIDE;
  bool ConvertPointFromWindow(CefPoint& point) OVERRIDE;
  bool ConvertPointToView(CefRefPtr<CefView> view, CefPoint& point) OVERRIDE;
  bool ConvertPointFromView(CefRefPtr<CefView> view, CefPoint& point) OVERRIDE;
};

#endif  // CEF_LIBCEF_DLL_CTOCPP_VIEWS_MENU_BUTTON_CTOCPP_H_
