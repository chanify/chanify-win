// 
// Utils.h
// Chanify
// 
// Created by WizJin.
//

#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once

#include "Chanify.h"
#include <string>
#include <memory>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

class Utils
{
private:
	HINSTANCE		m_hInstance;
	std::wstring	m_szModulePath;
public:
	explicit Utils(HINSTANCE hInstance) {
		m_hInstance = hInstance;
		m_szModulePath = _wpgmptr;
	}

	virtual ~Utils() {

	}

	inline HINSTANCE GetInstance(void) const { return m_hInstance; }

	inline std::wstring GetVersion(void) {
		std::wstring version;
		return version;
	}

	static std::shared_ptr<Utils>& Shared(void) {
		static std::shared_ptr<Utils> instance;
		return instance;
	}

	static void Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance) {
		auto ptr = std::make_shared<Utils>(hInstance);
		Shared().swap(ptr);
	}
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __UTILS_H__
