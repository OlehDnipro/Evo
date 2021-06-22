#pragma once

namespace NSH {


enum Root
{
	Samplers,
	Resources,
};

enum NResources
{
	SHAverage,
	EnvTex,
	OutTex,
	ShBase,
	ShSum,

};

const char* RootItemNames[2][5] =
{

	{
		"ColorSampler"
	},

	{
		"SHAverage",
		"EnvTex",
		"OutTex",
		"ShBase",
		"ShSum"
	},

};

const char* RootItemTypes[2][5] =
{

	{
		"SamplerState"
	},

	{
		"ShPoly3Channel ",
		"Texture2DArray<float4>",
		"RWTexture2DArray<float4>",
		"RWStructuredBuffer<ShBaseCoef>",
		"RWStructuredBuffer<ShPoly>"
	},

};

static const char RootSig[] =
	"\2\0\0\0\10\0\0\0\1\0\0\0\7\0\0\0\5\0\0\0\6\0\0\0\1\0\0\0\5\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\3\0\0\0\1\0\0\0\4\0\0\0\1\0\0\0\4\0\0\0"
	"\1\0\0\0";

static const char CSComputeBase[] =
	"\3\2#\7\0\0\1\0\0\0\16\0\23\1\0\0\0\0\0\0\21\0\2\0\1\0\0\0\13\0\6\0\1\0\0\0GLSL.std.450\0\0\0\0\16\0\3\0\0\0\0\0\1\0\0\0\17\0\6\0"
	"\5\0\0\0\2\0\0\0main\0\0\0\0\3\0\0\0\20\0\6\0\2\0\0\0\21\0\0\0 \0\0\0 \0\0\0\1\0\0\0\3\0\3\0\5\0\0\0X\2\0\0\5\0\4\0\4\0\0\0ShPol"
	"y\0\0\6\0\5\0\4\0\0\0\0\0\0\0coef\0\0\0\0\5\0\6\0\5\0\0\0type.sampler\0\0\0\0\5\0\6\0\6\0\0\0ColorSampler\0\0\0\0\5\0\5\0\7\0\0\0"
	"ShBaseCoef\0\0\6\0\5\0\7\0\0\0\0\0\0\0coef\0\0\0\0\5\0\7\0\10\0\0\0type.2d.image.array\0\5\0\4\0\11\0\0\0EnvTex\0\0\5\0\13\0\12\0"
	"\0\0type.RWStructuredBuffer.ShBaseCoef\0\0\5\0\4\0\13\0\0\0ShBase\0\0\5\0\12\0\14\0\0\0type.RWStructuredBuffer.ShPoly\0\0\5\0\4\0"
	"\15\0\0\0ShPoly\0\0\6\0\5\0\15\0\0\0\0\0\0\0coef\0\0\0\0\5\0\4\0\16\0\0\0ShSum\0\0\0\5\0\5\0\17\0\0\0ShSumTemp\0\0\0\5\0\4\0\2\0"
	"\0\0main\0\0\0\0\5\0\7\0\20\0\0\0type.sampled.image\0\0G\0\4\0\3\0\0\0\13\0\0\0\34\0\0\0G\0\4\0\6\0\0\0\"\0\0\0\0\0\0\0G\0\4\0\6"
	"\0\0\0!\0\0\0\0\0\0\0G\0\4\0\11\0\0\0\"\0\0\0\1\0\0\0G\0\4\0\11\0\0\0!\0\0\0\1\0\0\0G\0\4\0\13\0\0\0\"\0\0\0\1\0\0\0G\0\4\0\13\0"
	"\0\0!\0\0\0\3\0\0\0G\0\4\0\16\0\0\0\"\0\0\0\1\0\0\0G\0\4\0\16\0\0\0!\0\0\0\4\0\0\0G\0\4\0\21\0\0\0\6\0\0\0\20\0\0\0H\0\5\0\7\0\0"
	"\0\0\0\0\0#\0\0\0\0\0\0\0G\0\4\0\22\0\0\0\6\0\0\0\60\0\0\0H\0\5\0\12\0\0\0\0\0\0\0#\0\0\0\0\0\0\0G\0\3\0\12\0\0\0\3\0\0\0G\0\4\0"
	"\23\0\0\0\6\0\0\0\20\0\0\0H\0\5\0\15\0\0\0\0\0\0\0#\0\0\0\0\0\0\0G\0\4\0\24\0\0\0\6\0\0\0\60\0\0\0H\0\5\0\14\0\0\0\0\0\0\0#\0\0\0"
	"\0\0\0\0G\0\3\0\14\0\0\0\3\0\0\0\25\0\4\0\25\0\0\0 \0\0\0\0\0\0\0+\0\4\0\25\0\0\0\26\0\0\0\0\4\0\0\25\0\4\0\27\0\0\0 \0\0\0\1\0\0"
	"\0+\0\4\0\27\0\0\0\30\0\0\0\0\0\0\0+\0\4\0\27\0\0\0\31\0\0\0\1\0\0\0+\0\4\0\27\0\0\0\32\0\0\0\2\0\0\0+\0\4\0\25\0\0\0\33\0\0\0\6"
	"\0\0\0+\0\4\0\25\0\0\0\34\0\0\0\3\0\0\0\27\0\4\0\35\0\0\0\27\0\0\0\4\0\0\0\34\0\4\0\36\0\0\0\35\0\0\0\34\0\0\0\36\0\3\0\4\0\0\0\36"
	"\0\0\0.\0\3\0\4\0\0\0\37\0\0\0+\0\4\0\25\0\0\0 \0\0\0\1\0\0\0\26\0\3\0!\0\0\0 \0\0\0+\0\4\0!\0\0\0\"\0\0\0\0\0\200?+\0\4\0\25\0\0"
	"\0#\0\0\0\0\0\0\0+\0\4\0\25\0\0\0$\0\0\0\2\0\0\0+\0\4\0!\0\0\0%\0\0\0\301n\220>+\0\4\0!\0\0\0&\0\0\0,*\372>+\0\4\0\27\0\0\0'\0\0"
	"\0\3\0\0\0+\0\4\0!\0\0\0(\0\0\0\235\330\213?+\0\4\0!\0\0\0)\0\0\0\17{\241>+\0\4\0!\0\0\0*\0\0\0\0\0@@+\0\4\0!\0\0\0+\0\0\0\235\330"
	"\13?+\0\4\0!\0\0\0,\0\0\0\0\0\0\0+\0\4\0!\0\0\0-\0\0\0\0$tI+\0\4\0\25\0\0\0.\0\0\0 \0\0\0\32\0\2\0\5\0\0\0 \0\4\0/\0\0\0\0\0\0\0"
	"\5\0\0\0\27\0\4\0\60\0\0\0!\0\0\0\4\0\0\0\34\0\4\0\21\0\0\0\60\0\0\0\34\0\0\0\36\0\3\0\7\0\0\0\21\0\0\0\31\0\11\0\10\0\0\0!\0\0\0"
	"\1\0\0\0\2\0\0\0\1\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0 \0\4\0\61\0\0\0\0\0\0\0\10\0\0\0\35\0\3\0\22\0\0\0\7\0\0\0\36\0\3\0\12\0\0\0\22"
	"\0\0\0 \0\4\0\62\0\0\0\2\0\0\0\12\0\0\0\34\0\4\0\23\0\0\0\35\0\0\0\34\0\0\0\36\0\3\0\15\0\0\0\23\0\0\0\35\0\3\0\24\0\0\0\15\0\0\0"
	"\36\0\3\0\14\0\0\0\24\0\0\0 \0\4\0\63\0\0\0\2\0\0\0\14\0\0\0\34\0\4\0\64\0\0\0\4\0\0\0\34\0\0\0 \0\4\0\65\0\0\0\4\0\0\0\64\0\0\0"
	"\27\0\4\0\66\0\0\0\25\0\0\0\3\0\0\0 \0\4\0\67\0\0\0\1\0\0\0\66\0\0\0\23\0\2\0\70\0\0\0!\0\3\0\71\0\0\0\70\0\0\0\27\0\4\0:\0\0\0!"
	"\0\0\0\3\0\0\0 \0\4\0;\0\0\0\7\0\0\0!\0\0\0 \0\4\0<\0\0\0\7\0\0\0\60\0\0\0\24\0\2\0=\0\0\0 \0\4\0>\0\0\0\4\0\0\0\4\0\0\0+\0\4\0\25"
	"\0\0\0?\0\0\0\10\1\0\0 \0\4\0@\0\0\0\2\0\0\0\7\0\0\0\27\0\4\0A\0\0\0\27\0\0\0\3\0\0\0\33\0\3\0\20\0\0\0\10\0\0\0 \0\4\0B\0\0\0\4"
	"\0\0\0\27\0\0\0 \0\4\0C\0\0\0\2\0\0\0\15\0\0\0;\0\4\0/\0\0\0\6\0\0\0\0\0\0\0;\0\4\0\61\0\0\0\11\0\0\0\0\0\0\0;\0\4\0\62\0\0\0\13"
	"\0\0\0\2\0\0\0;\0\4\0\63\0\0\0\16\0\0\0\2\0\0\0;\0\4\0\65\0\0\0\17\0\0\0\4\0\0\0;\0\4\0\67\0\0\0\3\0\0\0\1\0\0\0\1\0\3\0\60\0\0\0"
	"D\0\0\0+\0\4\0!\0\0\0E\0\0\0\0\0\200\277+\0\4\0\25\0\0\0F\0\0\0\0\30\0\0+\0\4\0!\0\0\0G\0\0\0\10 \0;+\0\4\0\25\0\0\0H\0\0\0\0\0\20"
	"\0+\0\4\0!\0\0\0I\0\0\0\340\275\211H6\0\5\0\70\0\0\0\2\0\0\0\0\0\0\0\71\0\0\0\370\0\2\0J\0\0\0;\0\4\0<\0\0\0K\0\0\0\7\0\0\0=\0\4"
	"\0\66\0\0\0L\0\0\0\3\0\0\0\367\0\3\0M\0\0\0\0\0\0\0\373\0\3\0#\0\0\0N\0\0\0\370\0\2\0N\0\0\0Q\0\5\0\25\0\0\0O\0\0\0L\0\0\0\0\0\0"
	"\0Q\0\5\0\25\0\0\0P\0\0\0L\0\0\0\1\0\0\0Q\0\5\0\25\0\0\0Q\0\0\0L\0\0\0\2\0\0\0\256\0\5\0=\0\0\0R\0\0\0O\0\0\0\26\0\0\0\256\0\5\0"
	"=\0\0\0S\0\0\0P\0\0\0\26\0\0\0\246\0\5\0=\0\0\0T\0\0\0R\0\0\0S\0\0\0\256\0\5\0=\0\0\0U\0\0\0Q\0\0\0\33\0\0\0\246\0\5\0=\0\0\0V\0"
	"\0\0T\0\0\0U\0\0\0\367\0\3\0W\0\0\0\0\0\0\0\372\0\4\0V\0\0\0X\0\0\0W\0\0\0\370\0\2\0X\0\0\0\371\0\2\0M\0\0\0\370\0\2\0W\0\0\0A\0"
	"\5\0>\0\0\0Y\0\0\0\17\0\0\0\30\0\0\0>\0\3\0Y\0\0\0\37\0\0\0A\0\5\0>\0\0\0Z\0\0\0\17\0\0\0\31\0\0\0>\0\3\0Z\0\0\0\37\0\0\0A\0\5\0"
	">\0\0\0[\0\0\0\17\0\0\0\32\0\0\0>\0\3\0[\0\0\0\37\0\0\0\340\0\4\0$\0\0\0$\0\0\0?\0\0\0p\0\4\0!\0\0\0\\\0\0\0O\0\0\0\205\0\5\0!\0"
	"\0\0]\0\0\0\\\0\0\0G\0\0\0\203\0\5\0!\0\0\0^\0\0\0]\0\0\0\"\0\0\0p\0\4\0!\0\0\0_\0\0\0P\0\0\0\205\0\5\0!\0\0\0`\0\0\0_\0\0\0G\0\0"
	"\0\203\0\5\0!\0\0\0a\0\0\0`\0\0\0\"\0\0\0P\0\6\0:\0\0\0b\0\0\0^\0\0\0a\0\0\0E\0\0\0\14\0\6\0:\0\0\0c\0\0\0\1\0\0\0E\0\0\0b\0\0\0"
	"\367\0\3\0d\0\0\0\0\0\0\0\373\0\15\0Q\0\0\0e\0\0\0\2\0\0\0f\0\0\0\4\0\0\0g\0\0\0\5\0\0\0h\0\0\0\0\0\0\0i\0\0\0\1\0\0\0j\0\0\0\370"
	"\0\2\0f\0\0\0Q\0\5\0!\0\0\0k\0\0\0c\0\0\0\0\0\0\0\205\0\5\0!\0\0\0l\0\0\0k\0\0\0E\0\0\0R\0\6\0:\0\0\0m\0\0\0l\0\0\0c\0\0\0\0\0\0"
	"\0Q\0\5\0!\0\0\0n\0\0\0c\0\0\0\2\0\0\0\205\0\5\0!\0\0\0o\0\0\0n\0\0\0E\0\0\0R\0\6\0:\0\0\0p\0\0\0o\0\0\0m\0\0\0\2\0\0\0\371\0\2\0"
	"d\0\0\0\370\0\2\0g\0\0\0Q\0\5\0!\0\0\0q\0\0\0c\0\0\0\0\0\0\0Q\0\5\0!\0\0\0r\0\0\0c\0\0\0\2\0\0\0R\0\6\0:\0\0\0s\0\0\0r\0\0\0c\0\0"
	"\0\0\0\0\0\177\0\4\0!\0\0\0t\0\0\0q\0\0\0R\0\6\0:\0\0\0u\0\0\0t\0\0\0s\0\0\0\2\0\0\0\371\0\2\0d\0\0\0\370\0\2\0h\0\0\0Q\0\5\0!\0"
	"\0\0v\0\0\0c\0\0\0\0\0\0\0Q\0\5\0!\0\0\0w\0\0\0c\0\0\0\2\0\0\0\177\0\4\0!\0\0\0x\0\0\0w\0\0\0R\0\6\0:\0\0\0y\0\0\0x\0\0\0c\0\0\0"
	"\0\0\0\0R\0\6\0:\0\0\0z\0\0\0v\0\0\0y\0\0\0\2\0\0\0\371\0\2\0d\0\0\0\370\0\2\0i\0\0\0Q\0\5\0!\0\0\0{\0\0\0c\0\0\0\1\0\0\0Q\0\5\0"
	"!\0\0\0|\0\0\0c\0\0\0\2\0\0\0\177\0\4\0!\0\0\0}\0\0\0|\0\0\0R\0\6\0:\0\0\0~\0\0\0}\0\0\0c\0\0\0\1\0\0\0R\0\6\0:\0\0\0\177\0\0\0{"
	"\0\0\0~\0\0\0\2\0\0\0\371\0\2\0d\0\0\0\370\0\2\0j\0\0\0Q\0\5\0!\0\0\0\200\0\0\0c\0\0\0\1\0\0\0Q\0\5\0!\0\0\0\201\0\0\0c\0\0\0\2\0"
	"\0\0R\0\6\0:\0\0\0\202\0\0\0\201\0\0\0c\0\0\0\1\0\0\0\177\0\4\0!\0\0\0\203\0\0\0\200\0\0\0R\0\6\0:\0\0\0\204\0\0\0\203\0\0\0\202"
	"\0\0\0\2\0\0\0\371\0\2\0d\0\0\0\370\0\2\0e\0\0\0\371\0\2\0d\0\0\0\370\0\2\0d\0\0\0\365\0\17\0:\0\0\0\205\0\0\0p\0\0\0f\0\0\0u\0\0"
	"\0g\0\0\0z\0\0\0h\0\0\0\177\0\0\0i\0\0\0\204\0\0\0j\0\0\0c\0\0\0e\0\0\0\204\0\5\0\25\0\0\0\206\0\0\0P\0\0\0\26\0\0\0\200\0\5\0\25"
	"\0\0\0\207\0\0\0O\0\0\0\206\0\0\0\204\0\5\0\25\0\0\0\210\0\0\0Q\0\0\0H\0\0\0\200\0\5\0\25\0\0\0\211\0\0\0\207\0\0\0\210\0\0\0R\0"
	"\6\0\60\0\0\0\212\0\0\0%\0\0\0D\0\0\0\0\0\0\0Q\0\5\0!\0\0\0\213\0\0\0\205\0\0\0\1\0\0\0\205\0\5\0!\0\0\0\214\0\0\0&\0\0\0\213\0\0"
	"\0R\0\6\0\60\0\0\0\215\0\0\0\214\0\0\0\212\0\0\0\1\0\0\0Q\0\5\0!\0\0\0\216\0\0\0\205\0\0\0\2\0\0\0\205\0\5\0!\0\0\0\217\0\0\0&\0"
	"\0\0\216\0\0\0R\0\6\0\60\0\0\0\220\0\0\0\217\0\0\0\215\0\0\0\2\0\0\0Q\0\5\0!\0\0\0\221\0\0\0\205\0\0\0\0\0\0\0\205\0\5\0!\0\0\0\222"
	"\0\0\0&\0\0\0\221\0\0\0R\0\6\0\60\0\0\0\223\0\0\0\222\0\0\0\220\0\0\0\3\0\0\0\205\0\5\0!\0\0\0\224\0\0\0(\0\0\0\221\0\0\0\205\0\5"
	"\0!\0\0\0\225\0\0\0\224\0\0\0\213\0\0\0R\0\6\0\60\0\0\0\226\0\0\0\225\0\0\0D\0\0\0\0\0\0\0\205\0\5\0!\0\0\0\227\0\0\0(\0\0\0\213"
	"\0\0\0\205\0\5\0!\0\0\0\230\0\0\0\227\0\0\0\216\0\0\0R\0\6\0\60\0\0\0\231\0\0\0\230\0\0\0\226\0\0\0\1\0\0\0\205\0\5\0!\0\0\0\232"
	"\0\0\0*\0\0\0\216\0\0\0\205\0\5\0!\0\0\0\233\0\0\0\232\0\0\0\216\0\0\0\203\0\5\0!\0\0\0\234\0\0\0\233\0\0\0\"\0\0\0\205\0\5\0!\0"
	"\0\0\235\0\0\0)\0\0\0\234\0\0\0R\0\6\0\60\0\0\0\236\0\0\0\235\0\0\0\231\0\0\0\2\0\0\0\205\0\5\0!\0\0\0\237\0\0\0\224\0\0\0\216\0"
	"\0\0R\0\6\0\60\0\0\0\240\0\0\0\237\0\0\0\236\0\0\0\3\0\0\0\205\0\5\0!\0\0\0\241\0\0\0\221\0\0\0\221\0\0\0\205\0\5\0!\0\0\0\242\0"
	"\0\0\213\0\0\0\213\0\0\0\203\0\5\0!\0\0\0\243\0\0\0\241\0\0\0\242\0\0\0\205\0\5\0!\0\0\0\244\0\0\0+\0\0\0\243\0\0\0R\0\6\0\60\0\0"
	"\0\245\0\0\0\244\0\0\0D\0\0\0\0\0\0\0A\0\6\0@\0\0\0\246\0\0\0\13\0\0\0\30\0\0\0\211\0\0\0P\0\6\0\21\0\0\0\247\0\0\0\223\0\0\0\240"
	"\0\0\0\245\0\0\0P\0\4\0\7\0\0\0\250\0\0\0\247\0\0\0>\0\3\0\246\0\0\0\250\0\0\0=\0\4\0\10\0\0\0\251\0\0\0\11\0\0\0=\0\4\0\5\0\0\0"
	"\252\0\0\0\6\0\0\0|\0\4\0\27\0\0\0\253\0\0\0O\0\0\0|\0\4\0\27\0\0\0\254\0\0\0P\0\0\0|\0\4\0\27\0\0\0\255\0\0\0Q\0\0\0P\0\6\0A\0\0"
	"\0\256\0\0\0\253\0\0\0\254\0\0\0\255\0\0\0o\0\4\0:\0\0\0\257\0\0\0\256\0\0\0V\0\5\0\20\0\0\0\260\0\0\0\251\0\0\0\252\0\0\0X\0\7\0"
	"\60\0\0\0\261\0\0\0\260\0\0\0\257\0\0\0\2\0\0\0,\0\0\0>\0\3\0K\0\0\0\261\0\0\0\371\0\2\0\262\0\0\0\370\0\2\0\262\0\0\0\365\0\7\0"
	"\27\0\0\0\263\0\0\0\30\0\0\0d\0\0\0\264\0\0\0\265\0\0\0\261\0\5\0=\0\0\0\266\0\0\0\263\0\0\0'\0\0\0\366\0\4\0\267\0\0\0\265\0\0\0"
	"\1\0\0\0\372\0\4\0\266\0\0\0\265\0\0\0\267\0\0\0\370\0\2\0\265\0\0\0A\0\10\0B\0\0\0\270\0\0\0\17\0\0\0\263\0\0\0\30\0\0\0\30\0\0"
	"\0\30\0\0\0|\0\4\0\25\0\0\0\271\0\0\0\263\0\0\0A\0\5\0;\0\0\0\272\0\0\0K\0\0\0\271\0\0\0=\0\4\0!\0\0\0\273\0\0\0\272\0\0\0\205\0"
	"\5\0!\0\0\0\274\0\0\0\273\0\0\0I\0\0\0m\0\4\0\25\0\0\0\275\0\0\0\274\0\0\0|\0\4\0\27\0\0\0\276\0\0\0\275\0\0\0\352\0\7\0\27\0\0\0"
	"\277\0\0\0\270\0\0\0 \0\0\0#\0\0\0\276\0\0\0A\0\10\0B\0\0\0\300\0\0\0\17\0\0\0\263\0\0\0\30\0\0\0\30\0\0\0\31\0\0\0=\0\4\0!\0\0\0"
	"\301\0\0\0\272\0\0\0\205\0\5\0!\0\0\0\302\0\0\0-\0\0\0\301\0\0\0\205\0\5\0!\0\0\0\303\0\0\0\302\0\0\0\214\0\0\0m\0\4\0\25\0\0\0\304"
	"\0\0\0\303\0\0\0|\0\4\0\27\0\0\0\305\0\0\0\304\0\0\0\352\0\7\0\27\0\0\0\306\0\0\0\300\0\0\0 \0\0\0#\0\0\0\305\0\0\0A\0\10\0B\0\0"
	"\0\307\0\0\0\17\0\0\0\263\0\0\0\30\0\0\0\30\0\0\0\32\0\0\0=\0\4\0!\0\0\0\310\0\0\0\272\0\0\0\205\0\5\0!\0\0\0\311\0\0\0-\0\0\0\310"
	"\0\0\0\205\0\5\0!\0\0\0\312\0\0\0\311\0\0\0\217\0\0\0m\0\4\0\25\0\0\0\313\0\0\0\312\0\0\0|\0\4\0\27\0\0\0\314\0\0\0\313\0\0\0\352"
	"\0\7\0\27\0\0\0\315\0\0\0\307\0\0\0 \0\0\0#\0\0\0\314\0\0\0A\0\10\0B\0\0\0\316\0\0\0\17\0\0\0\263\0\0\0\30\0\0\0\30\0\0\0'\0\0\0"
	"=\0\4\0!\0\0\0\317\0\0\0\272\0\0\0\205\0\5\0!\0\0\0\320\0\0\0-\0\0\0\317\0\0\0\205\0\5\0!\0\0\0\321\0\0\0\320\0\0\0\222\0\0\0m\0"
	"\4\0\25\0\0\0\322\0\0\0\321\0\0\0|\0\4\0\27\0\0\0\323\0\0\0\322\0\0\0\352\0\7\0\27\0\0\0\324\0\0\0\316\0\0\0 \0\0\0#\0\0\0\323\0"
	"\0\0A\0\10\0B\0\0\0\325\0\0\0\17\0\0\0\263\0\0\0\30\0\0\0\31\0\0\0\30\0\0\0=\0\4\0!\0\0\0\326\0\0\0\272\0\0\0\205\0\5\0!\0\0\0\327"
	"\0\0\0-\0\0\0\326\0\0\0\205\0\5\0!\0\0\0\330\0\0\0\327\0\0\0\225\0\0\0m\0\4\0\25\0\0\0\331\0\0\0\330\0\0\0|\0\4\0\27\0\0\0\332\0"
	"\0\0\331\0\0\0\352\0\7\0\27\0\0\0\333\0\0\0\325\0\0\0 \0\0\0#\0\0\0\332\0\0\0A\0\10\0B\0\0\0\334\0\0\0\17\0\0\0\263\0\0\0\30\0\0"
	"\0\31\0\0\0\31\0\0\0=\0\4\0!\0\0\0\335\0\0\0\272\0\0\0\205\0\5\0!\0\0\0\336\0\0\0-\0\0\0\335\0\0\0\205\0\5\0!\0\0\0\337\0\0\0\336"
	"\0\0\0\230\0\0\0m\0\4\0\25\0\0\0\340\0\0\0\337\0\0\0|\0\4\0\27\0\0\0\341\0\0\0\340\0\0\0\352\0\7\0\27\0\0\0\342\0\0\0\334\0\0\0 "
	"\0\0\0#\0\0\0\341\0\0\0A\0\10\0B\0\0\0\343\0\0\0\17\0\0\0\263\0\0\0\30\0\0\0\31\0\0\0\32\0\0\0=\0\4\0!\0\0\0\344\0\0\0\272\0\0\0"
	"\205\0\5\0!\0\0\0\345\0\0\0-\0\0\0\344\0\0\0\205\0\5\0!\0\0\0\346\0\0\0\345\0\0\0\235\0\0\0m\0\4\0\25\0\0\0\347\0\0\0\346\0\0\0|"
	"\0\4\0\27\0\0\0\350\0\0\0\347\0\0\0\352\0\7\0\27\0\0\0\351\0\0\0\343\0\0\0 \0\0\0#\0\0\0\350\0\0\0A\0\10\0B\0\0\0\352\0\0\0\17\0"
	"\0\0\263\0\0\0\30\0\0\0\31\0\0\0'\0\0\0=\0\4\0!\0\0\0\353\0\0\0\272\0\0\0\205\0\5\0!\0\0\0\354\0\0\0-\0\0\0\353\0\0\0\205\0\5\0!"
	"\0\0\0\355\0\0\0\354\0\0\0\237\0\0\0m\0\4\0\25\0\0\0\356\0\0\0\355\0\0\0|\0\4\0\27\0\0\0\357\0\0\0\356\0\0\0\352\0\7\0\27\0\0\0\360"
	"\0\0\0\352\0\0\0 \0\0\0#\0\0\0\357\0\0\0A\0\10\0B\0\0\0\361\0\0\0\17\0\0\0\263\0\0\0\30\0\0\0\32\0\0\0\30\0\0\0=\0\4\0!\0\0\0\362"
	"\0\0\0\272\0\0\0\205\0\5\0!\0\0\0\363\0\0\0-\0\0\0\362\0\0\0\205\0\5\0!\0\0\0\364\0\0\0\363\0\0\0\244\0\0\0m\0\4\0\25\0\0\0\365\0"
	"\0\0\364\0\0\0|\0\4\0\27\0\0\0\366\0\0\0\365\0\0\0\352\0\7\0\27\0\0\0\367\0\0\0\361\0\0\0 \0\0\0#\0\0\0\366\0\0\0A\0\10\0B\0\0\0"
	"\370\0\0\0\17\0\0\0\263\0\0\0\30\0\0\0\32\0\0\0\31\0\0\0|\0\4\0\27\0\0\0\371\0\0\0 \0\0\0\352\0\7\0\27\0\0\0\372\0\0\0\370\0\0\0"
	" \0\0\0#\0\0\0\371\0\0\0\200\0\5\0\27\0\0\0\264\0\0\0\263\0\0\0\31\0\0\0\371\0\2\0\262\0\0\0\370\0\2\0\267\0\0\0\340\0\4\0$\0\0\0"
	"$\0\0\0?\0\0\0\206\0\5\0\25\0\0\0\373\0\0\0O\0\0\0.\0\0\0\206\0\5\0\25\0\0\0\374\0\0\0P\0\0\0.\0\0\0\204\0\5\0\25\0\0\0\375\0\0\0"
	"\374\0\0\0.\0\0\0\200\0\5\0\25\0\0\0\376\0\0\0\373\0\0\0\375\0\0\0\204\0\5\0\25\0\0\0\377\0\0\0Q\0\0\0\26\0\0\0\200\0\5\0\25\0\0"
	"\0\0\1\0\0\376\0\0\0\377\0\0\0\371\0\2\0\1\1\0\0\370\0\2\0\1\1\0\0\365\0\7\0\27\0\0\0\2\1\0\0\30\0\0\0\267\0\0\0\3\1\0\0\4\1\0\0"
	"\261\0\5\0=\0\0\0\5\1\0\0\2\1\0\0'\0\0\0\366\0\4\0\6\1\0\0\4\1\0\0\0\0\0\0\372\0\4\0\5\1\0\0\4\1\0\0\6\1\0\0\370\0\2\0\4\1\0\0A\0"
	"\5\0>\0\0\0\7\1\0\0\17\0\0\0\2\1\0\0=\0\4\0\4\0\0\0\10\1\0\0\7\1\0\0|\0\4\0\25\0\0\0\11\1\0\0\2\1\0\0\204\0\5\0\25\0\0\0\12\1\0\0"
	"\11\1\0\0F\0\0\0\200\0\5\0\25\0\0\0\13\1\0\0\12\1\0\0\0\1\0\0A\0\6\0C\0\0\0\14\1\0\0\16\0\0\0\30\0\0\0\13\1\0\0Q\0\5\0\36\0\0\0\15"
	"\1\0\0\10\1\0\0\0\0\0\0Q\0\5\0\35\0\0\0\16\1\0\0\15\1\0\0\0\0\0\0Q\0\5\0\35\0\0\0\17\1\0\0\15\1\0\0\1\0\0\0Q\0\5\0\35\0\0\0\20\1"
	"\0\0\15\1\0\0\2\0\0\0P\0\6\0\23\0\0\0\21\1\0\0\16\1\0\0\17\1\0\0\20\1\0\0P\0\4\0\15\0\0\0\22\1\0\0\21\1\0\0>\0\3\0\14\1\0\0\22\1"
	"\0\0\200\0\5\0\27\0\0\0\3\1\0\0\2\1\0\0\31\0\0\0\371\0\2\0\1\1\0\0\370\0\2\0\6\1\0\0\371\0\2\0M\0\0\0\370\0\2\0M\0\0\0\375\0\1\0"
	"\70\0\1\0";

static const char CSComputeTex[] =
	"\3\2#\7\0\0\1\0\0\0\16\0#\0\0\0\0\0\0\0\21\0\2\0\1\0\0\0\16\0\3\0\0\0\0\0\1\0\0\0\17\0\6\0\5\0\0\0\1\0\0\0main\0\0\0\0\2\0\0\0\20"
	"\0\6\0\1\0\0\0\21\0\0\0 \0\0\0 \0\0\0\1\0\0\0\3\0\3\0\5\0\0\0X\2\0\0\5\0\7\0\3\0\0\0type.2d.image.array\0\5\0\4\0\4\0\0\0OutTex\0"
	"\0\5\0\4\0\1\0\0\0main\0\0\0\0G\0\4\0\2\0\0\0\13\0\0\0\34\0\0\0G\0\4\0\4\0\0\0\"\0\0\0\1\0\0\0G\0\4\0\4\0\0\0!\0\0\0\2\0\0\0\25\0"
	"\4\0\5\0\0\0 \0\0\0\0\0\0\0+\0\4\0\5\0\0\0\6\0\0\0\0\4\0\0+\0\4\0\5\0\0\0\7\0\0\0\6\0\0\0\26\0\3\0\10\0\0\0 \0\0\0+\0\4\0\10\0\0"
	"\0\11\0\0\0\0\0\0\0+\0\4\0\10\0\0\0\12\0\0\0\0\0\200?\27\0\4\0\13\0\0\0\10\0\0\0\4\0\0\0,\0\7\0\13\0\0\0\14\0\0\0\12\0\0\0\11\0\0"
	"\0\12\0\0\0\12\0\0\0\31\0\11\0\3\0\0\0\10\0\0\0\1\0\0\0\2\0\0\0\1\0\0\0\0\0\0\0\2\0\0\0\1\0\0\0 \0\4\0\15\0\0\0\0\0\0\0\3\0\0\0\27"
	"\0\4\0\16\0\0\0\5\0\0\0\3\0\0\0 \0\4\0\17\0\0\0\1\0\0\0\16\0\0\0\23\0\2\0\20\0\0\0!\0\3\0\21\0\0\0\20\0\0\0\24\0\2\0\22\0\0\0;\0"
	"\4\0\15\0\0\0\4\0\0\0\0\0\0\0;\0\4\0\17\0\0\0\2\0\0\0\1\0\0\0+\0\4\0\5\0\0\0\23\0\0\0\0\0\0\0\66\0\5\0\20\0\0\0\1\0\0\0\0\0\0\0\21"
	"\0\0\0\370\0\2\0\24\0\0\0=\0\4\0\16\0\0\0\25\0\0\0\2\0\0\0\367\0\3\0\26\0\0\0\0\0\0\0\373\0\3\0\23\0\0\0\27\0\0\0\370\0\2\0\27\0"
	"\0\0Q\0\5\0\5\0\0\0\30\0\0\0\25\0\0\0\0\0\0\0Q\0\5\0\5\0\0\0\31\0\0\0\25\0\0\0\1\0\0\0Q\0\5\0\5\0\0\0\32\0\0\0\25\0\0\0\2\0\0\0\256"
	"\0\5\0\22\0\0\0\33\0\0\0\30\0\0\0\6\0\0\0\256\0\5\0\22\0\0\0\34\0\0\0\31\0\0\0\6\0\0\0\246\0\5\0\22\0\0\0\35\0\0\0\33\0\0\0\34\0"
	"\0\0\256\0\5\0\22\0\0\0\36\0\0\0\32\0\0\0\7\0\0\0\246\0\5\0\22\0\0\0\37\0\0\0\35\0\0\0\36\0\0\0\367\0\3\0 \0\0\0\0\0\0\0\372\0\4"
	"\0\37\0\0\0!\0\0\0 \0\0\0\370\0\2\0!\0\0\0\371\0\2\0\26\0\0\0\370\0\2\0 \0\0\0=\0\4\0\3\0\0\0\"\0\0\0\4\0\0\0c\0\5\0\"\0\0\0\25\0"
	"\0\0\14\0\0\0\0\0\0\0\371\0\2\0\26\0\0\0\370\0\2\0\26\0\0\0\375\0\1\0\70\0\1\0";

}; // namespace NSH
