/******************** (C) Copyright 2021 realworld one GmbH & Co. KG All Rights Reserved. ********************/

#include "UrCefEnums.h"

#include "CefIncludesStart.h"
#include <include/internal/cef_types.h>
#include "CefIncludesEnd.h"

TMap<int, FString> FUrCefErrorCode::ErrorCodes = {
	{0, TEXT("NONE")},
	{-2, TEXT("FAILED")},
	{-3, TEXT("ABORTED")},
	{-4, TEXT("INVALID_ARGUMENT")},
	{-5, TEXT("INVALID_HANDLE")},
	{-6, TEXT("FILE_NOT_FOUND")},
	{-7, TEXT("TIMED_OUT")},
	{-8, TEXT("FILE_TOO_BIG")},
	{-9, TEXT("UNEXPECTED")},
	{-10, TEXT("ACCESS_DENIED")},
	{-11, TEXT("NOT_IMPLEMENTED")},
	{-100, TEXT("CONNECTION_CLOSED")},
	{-101, TEXT("CONNECTION_RESET")},
	{-102, TEXT("CONNECTION_REFUSED")},
	{-103, TEXT("CONNECTION_ABORTED")},
	{-104, TEXT("CONNECTION_FAILED")},
	{-105, TEXT("NAME_NOT_RESOLVED")},
	{-106, TEXT("INTERNET_DISCONNECTED")},
	{-107, TEXT("SSL_PROTOCOL_ERROR")},
	{-108, TEXT("ADDRESS_INVALID")},
	{-109, TEXT("ADDRESS_UNREACHABLE")},
	{-110, TEXT("SSL_CLIENT_AUTH_CERT_NEEDED")},
	{-111, TEXT("TUNNEL_CONNECTION_FAILED")},
	{-112, TEXT("NO_SSL_VERSIONS_ENABLED")},
	{-113, TEXT("SSL_VERSION_OR_CIPHER_MISMATCH")},
	{-114, TEXT("SSL_RENEGOTIATION_REQUESTED")},
	{-200, TEXT("CERT_COMMON_NAME_INVALID")},
	{-201, TEXT("CERT_DATE_INVALID")},
	{-202, TEXT("CERT_AUTHORITY_INVALID")},
	{-203, TEXT("CERT_CONTAINS_ERRORS")},
	{-204, TEXT("CERT_NO_REVOCATION_MECHANISM")},
	{-205, TEXT("CERT_UNABLE_TO_CHECK_REVOCATION")},
	{-206, TEXT("CERT_REVOKED")},
	{-207, TEXT("CERT_INVALID")},
	{-208, TEXT("CERT_WEAK_SIGNATURE_ALGORITHM")},
	// -209 is available: was CERT_NOT_IN_DNS.
	{-210, TEXT("CERT_NON_UNIQUE_NAME")},
	{-211, TEXT("CERT_WEAK_KEY")},
	{-212, TEXT("CERT_NAME_CONSTRAINT_VIOLATION")},
	{-213, TEXT("CERT_VALIDITY_TOO_LONG")},
	{-300, TEXT("INVALID_URL")},
	{-301, TEXT("DISALLOWED_URL_SCHEME")},
	{-302, TEXT("UNKNOWN_URL_SCHEME")},
	{-310, TEXT("TOO_MANY_REDIRECTS")},
	{-311, TEXT("UNSAFE_REDIRECT")},
	{-312, TEXT("UNSAFE_PORT")},
	{-320, TEXT("INVALID_RESPONSE")},
	{-321, TEXT("INVALID_CHUNKED_ENCODING")},
	{-322, TEXT("METHOD_NOT_SUPPORTED")},
	{-323, TEXT("UNEXPECTED_PROXY_AUTH")},
	{-324, TEXT("EMPTY_RESPONSE")},
	{-325, TEXT("RESPONSE_HEADERS_TOO_BIG")},
	{-400, TEXT("CACHE_MISS")},
	{-501, TEXT("INSECURE_RESPONSE")}
};

uint32 FCefKeyEventFlags::GetEventFlags() const
{
	return EVENTFLAG_NONE
		| EVENTFLAG_CAPS_LOCK_ON * CapsLocked
		| EVENTFLAG_SHIFT_DOWN * ShiftDown
		| EVENTFLAG_CONTROL_DOWN * ControlDown
		| EVENTFLAG_ALT_DOWN * AltDown
		| EVENTFLAG_LEFT_MOUSE_BUTTON * LeftMouseButton
		| EVENTFLAG_MIDDLE_MOUSE_BUTTON * MiddleMouseButton
		| EVENTFLAG_RIGHT_MOUSE_BUTTON * RightMouseButton
		| EVENTFLAG_COMMAND_DOWN * CommandDown
		| EVENTFLAG_NUM_LOCK_ON * NumLocked
		| EVENTFLAG_IS_KEY_PAD * IsKeypad
		| EVENTFLAG_IS_LEFT * Left
		| EVENTFLAG_IS_RIGHT * Right
		| EVENTFLAG_ALTGR_DOWN * AltGrDown
		| OrFlag;
}