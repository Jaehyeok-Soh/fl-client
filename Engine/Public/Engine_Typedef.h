#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		char						_char;
	typedef		int8_t						_byte;
	typedef		uint8_t						_ubyte;

	typedef		wchar_t						_tchar;
	
	typedef		wstring						_wstring;
	typedef		string						_string;

	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		unsigned long long			_uint64;

	typedef		float						_float;
	typedef		double						_double;
	
	using Vec2		= DirectX::SimpleMath::Vector2;
	using Vec3		= DirectX::SimpleMath::Vector3;
	using Vec4		= DirectX::SimpleMath::Vector4;
	using Matrix	= DirectX::SimpleMath::Matrix;
	using Quat		= DirectX::SimpleMath::Quaternion;

	// 기능 typedef
	using MinMax	= DirectX::SimpleMath::Vector2; // 변수 앞에 M 붙이기
	using TimeCount = DirectX::SimpleMath::Vector2; // 변수 앞에 T 붙이기
	using Flags		= signed int;					// 변수 앞에 F 붙이기
}

#endif // Engine_Typedef_h__
