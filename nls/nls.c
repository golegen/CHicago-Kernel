// File author is Ítalo Lima Marconato Matias
//
// Created on December 09 of 2018, at 19:27 BRT
// Last edited on December 09 of 2018, at 20:40 BRT

#include <chicago/nls.h>
#include <chicago/string.h>

extern PWChar NlsMessagesEn[];
extern PWChar NlsMessagesBr[];

UIntPtr NlsCurrentLanguage = NLS_LANG_EN;

PWChar NlsGetMessage(UIntPtr msg) {
	if (NlsCurrentLanguage == NLS_LANG_EN) {										// English?
		return NlsMessagesEn[msg];													// Yes!
	} else {
		return NlsMessagesBr[msg];													// No, so it's Portugueses (Brazil)
	}
}

PWChar NlsGetLanguages(Void) {
	return L"br, en";
}

Void NlsSetLanguage(UIntPtr lang) {
	if ((lang != NLS_LANG_EN) && (lang != NLS_LANG_BR)) {							// Valid language?
		NlsCurrentLanguage = NLS_LANG_EN;											// No :(
	} else {
		NlsCurrentLanguage = lang;													// Yes :)
	}
}

UIntPtr NlsGetLanguage(PWChar lang) {
	if (lang == Null) {																// Sanity check
		return (UIntPtr)-1;															// ...
	} else if ((StrGetLength(lang) == 2) && StrCompare(lang, L"br")) {				// Portuguese (Brazil)?
		return NLS_LANG_BR;	
	} else if ((StrGetLength(lang) == 2) && StrCompare(lang, L"en")) {				// English?
		return NLS_LANG_EN;	
	} else {
		return (UIntPtr)-1;
	}
}
