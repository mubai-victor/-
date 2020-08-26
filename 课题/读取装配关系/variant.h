//头文件
#pragma once
template <class T, int type> class SafeArray
{
public:
	SafeArray(VARIANT *input) :
		m_input(input),
		m_access(false),
		m_bCreate(false),
		m_pSafeArray(NULL),
		m_arrayData(NULL),
		m_result(S_OK)
	{
		if (m_input != NULL)
		{
			if (V_VT(m_input) != VT_EMPTY)
			{
				m_pSafeArray = V_ARRAY(m_input);
				m_result = SafeArrayAccessData(m_pSafeArray, (void HUGEP**)&m_arrayData);
				m_access = m_result == S_OK;
			}
			else
				m_access = true;
		}
		else
		{
			ASSERT(FALSE);
			m_input = &m_target;
		}
	}
	SafeArray(const VARIANT &input) :
		m_access(true),
		m_bCreate(false),
		m_pSafeArray(NULL),
		m_arrayData(NULL),
		m_result(S_OK)
	{
		m_input = (VARIANT *)&input;
		if (V_VT(m_input) != VT_EMPTY)
		{
			m_pSafeArray = V_ARRAY(m_input);
			m_result = SafeArrayAccessData(m_pSafeArray, (void HUGEP**)&m_arrayData);
			m_access = m_result == S_OK;
		}
		else
			m_access = true;
	}
	SafeArray(unsigned int size, unsigned int dims = 1) :
		m_input(&m_target),
		m_access(false),
		m_bCreate(true),
		m_pSafeArray(NULL),
		m_arrayData(NULL),
		m_result(S_OK)
	{
		ASSERT(size >= 0);
		if (size > 0)
		{
			m_rgsabound[0].lLbound = 0;
			m_rgsabound[0].cElements = (int)size;
			m_pSafeArray = SafeArrayCreate(type, (int)dims, m_rgsabound);
			V_VT(m_input) = VT_ARRAY | type;
			m_result = SafeArrayAccessData(m_pSafeArray, (void HUGEP**)&m_arrayData);
			m_access = m_result == S_OK;
			if (!m_access)
				V_VT(m_input) = VT_EMPTY;
		}
		else
		{
			V_VT(m_input) = VT_EMPTY;
			m_access = true;
		}
	}
	inline HRESULT status() { return m_result; }
	inline int getSize(int index = 0) { return (m_access&&m_pSafeArray) ? m_pSafeArray->rgsabound[index].cElements : 0; }
	~SafeArray() { UnaccessData(); }
	operator T* () { ASSERT(m_access); return m_arrayData; }
	operator VARIANT () { ASSERT(m_access); UnaccessData(); return *m_input; }
	T & operator[](int i) { ASSERT(m_access&&m_arrayData); return m_arrayData[i]; }
private:
	void UnaccessData() {
		if (m_access) {
			m_result = SafeArrayUnaccessData(m_pSafeArray);
			if (m_bCreate && m_result == S_OK)
				V_ARRAY(m_input) = m_pSafeArray;
			m_access = false;
		}
	}
	bool	m_bCreate;
	bool	m_access;
	VARIANT	*m_input;
	VARIANT	m_target;
	T	*m_arrayData;
	SAFEARRAY	*m_pSafeArray;
	SAFEARRAYBOUND m_rgsabound[1];
	HRESULT	m_result;
};
typedef SafeArray<double, VT_R8> SafeDoubleArray;
typedef SafeArray<long, VT_I4> SafeLongArray;
typedef SafeArray<BSTR, VT_BSTR> SafeBSTRArray;
typedef SafeArray<LPDISPATCH, VT_DISPATCH> SafeDISPATCHArray;
typedef SafeArray<LPVARIANT, VT_VARIANT> SafeVARIANTArray;