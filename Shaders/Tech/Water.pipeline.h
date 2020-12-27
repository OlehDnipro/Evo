#pragma once

namespace NWater {


enum Root
{
	Samplers,
	Resources,
};

enum NResources
{
	WaterConst,
	WaveTex,
	EnvTex,

};

const char* RootItemNames[2][3] =
{

	{
		"ColorSampler"
	},

	{
		"WaterConst",
		"WaveTex",
		"EnvTex"
	},

};

static const char RootSig[] =
	"\2\0\0\0\10\0\0\0\1\0\0\0\7\0\0\0\3\0\0\0\6\0\0\0\1\0\0\0\5\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0";

static const char VS[] =
	"\3\2#\7\0\0\1\0\0\0\16\0%\0\0\0\0\0\0\0\21\0\2\0\1\0\0\0\16\0\3\0\0\0\0\0\1\0\0\0\17\0\12\0\0\0\0\0\1\0\0\0main\0\0\0\0\2\0\0\0\3"
	"\0\0\0\4\0\0\0\5\0\0\0\6\0\0\0\3\0\3\0\5\0\0\0X\2\0\0\5\0\7\0\7\0\0\0type.cbWaterConst\0\0\0\6\0\6\0\7\0\0\0\0\0\0\0WaterConst\0"
	"\0\5\0\5\0\10\0\0\0SWaterConst\0\6\0\4\0\10\0\0\0\0\0\0\0mvp\0\6\0\5\0\10\0\0\0\1\0\0\0camPos\0\0\6\0\5\0\10\0\0\0\2\0\0\0pad1\0"
	"\0\0\0\6\0\5\0\10\0\0\0\3\0\0\0boxMax\0\0\6\0\5\0\10\0\0\0\4\0\0\0pad2\0\0\0\0\6\0\5\0\10\0\0\0\5\0\0\0boxMin\0\0\6\0\5\0\10\0\0"
	"\0\6\0\0\0pad3\0\0\0\0\6\0\6\0\10\0\0\0\7\0\0\0cubeCenter\0\0\6\0\5\0\10\0\0\0\10\0\0\0pad4\0\0\0\0\6\0\6\0\10\0\0\0\11\0\0\0box"
	"MaxFar\0\0\0\6\0\5\0\10\0\0\0\12\0\0\0pad5\0\0\0\0\6\0\6\0\10\0\0\0\13\0\0\0boxMinFar\0\0\0\6\0\5\0\10\0\0\0\14\0\0\0pad6\0\0\0\0"
	"\5\0\6\0\11\0\0\0cbWaterConst\0\0\0\0\5\0\6\0\2\0\0\0in.var.Position\0\5\0\6\0\3\0\0\0in.var.TexCoord\0\5\0\7\0\5\0\0\0out.var.T"
	"exCoord0\0\0\0\5\0\7\0\6\0\0\0out.var.Position0\0\0\0\5\0\4\0\1\0\0\0main\0\0\0\0G\0\4\0\4\0\0\0\13\0\0\0\0\0\0\0G\0\4\0\2\0\0\0"
	"\36\0\0\0\0\0\0\0G\0\4\0\3\0\0\0\36\0\0\0\1\0\0\0G\0\4\0\5\0\0\0\36\0\0\0\0\0\0\0G\0\4\0\6\0\0\0\36\0\0\0\1\0\0\0G\0\4\0\11\0\0\0"
	"\"\0\0\0\1\0\0\0G\0\4\0\11\0\0\0!\0\0\0\0\0\0\0H\0\5\0\10\0\0\0\0\0\0\0#\0\0\0\0\0\0\0H\0\5\0\10\0\0\0\0\0\0\0\7\0\0\0\20\0\0\0H"
	"\0\4\0\10\0\0\0\0\0\0\0\5\0\0\0H\0\5\0\10\0\0\0\1\0\0\0#\0\0\0@\0\0\0H\0\5\0\10\0\0\0\2\0\0\0#\0\0\0L\0\0\0H\0\5\0\10\0\0\0\3\0\0"
	"\0#\0\0\0P\0\0\0H\0\5\0\10\0\0\0\4\0\0\0#\0\0\0\\\0\0\0H\0\5\0\10\0\0\0\5\0\0\0#\0\0\0`\0\0\0H\0\5\0\10\0\0\0\6\0\0\0#\0\0\0l\0\0"
	"\0H\0\5\0\10\0\0\0\7\0\0\0#\0\0\0p\0\0\0H\0\5\0\10\0\0\0\10\0\0\0#\0\0\0|\0\0\0H\0\5\0\10\0\0\0\11\0\0\0#\0\0\0\200\0\0\0H\0\5\0"
	"\10\0\0\0\12\0\0\0#\0\0\0\214\0\0\0H\0\5\0\10\0\0\0\13\0\0\0#\0\0\0\220\0\0\0H\0\5\0\10\0\0\0\14\0\0\0#\0\0\0\234\0\0\0H\0\5\0\7"
	"\0\0\0\0\0\0\0#\0\0\0\0\0\0\0G\0\3\0\7\0\0\0\2\0\0\0\25\0\4\0\12\0\0\0 \0\0\0\1\0\0\0+\0\4\0\12\0\0\0\13\0\0\0\0\0\0\0\26\0\3\0\14"
	"\0\0\0 \0\0\0+\0\4\0\14\0\0\0\15\0\0\0\0\0\200?\27\0\4\0\16\0\0\0\14\0\0\0\4\0\0\0\30\0\4\0\17\0\0\0\16\0\0\0\4\0\0\0\27\0\4\0\20"
	"\0\0\0\14\0\0\0\3\0\0\0\36\0\17\0\10\0\0\0\17\0\0\0\20\0\0\0\14\0\0\0\20\0\0\0\14\0\0\0\20\0\0\0\14\0\0\0\20\0\0\0\14\0\0\0\20\0"
	"\0\0\14\0\0\0\20\0\0\0\14\0\0\0\36\0\3\0\7\0\0\0\10\0\0\0 \0\4\0\21\0\0\0\2\0\0\0\7\0\0\0 \0\4\0\22\0\0\0\1\0\0\0\20\0\0\0\27\0\4"
	"\0\23\0\0\0\14\0\0\0\2\0\0\0 \0\4\0\24\0\0\0\1\0\0\0\23\0\0\0 \0\4\0\25\0\0\0\3\0\0\0\16\0\0\0 \0\4\0\26\0\0\0\3\0\0\0\23\0\0\0 "
	"\0\4\0\27\0\0\0\3\0\0\0\20\0\0\0\23\0\2\0\30\0\0\0!\0\3\0\31\0\0\0\30\0\0\0 \0\4\0\32\0\0\0\2\0\0\0\17\0\0\0;\0\4\0\21\0\0\0\11\0"
	"\0\0\2\0\0\0;\0\4\0\22\0\0\0\2\0\0\0\1\0\0\0;\0\4\0\24\0\0\0\3\0\0\0\1\0\0\0;\0\4\0\25\0\0\0\4\0\0\0\3\0\0\0;\0\4\0\26\0\0\0\5\0"
	"\0\0\3\0\0\0;\0\4\0\27\0\0\0\6\0\0\0\3\0\0\0\66\0\5\0\30\0\0\0\1\0\0\0\0\0\0\0\31\0\0\0\370\0\2\0\33\0\0\0=\0\4\0\20\0\0\0\34\0\0"
	"\0\2\0\0\0=\0\4\0\23\0\0\0\35\0\0\0\3\0\0\0A\0\6\0\32\0\0\0\36\0\0\0\11\0\0\0\13\0\0\0\13\0\0\0=\0\4\0\17\0\0\0\37\0\0\0\36\0\0\0"
	"Q\0\5\0\14\0\0\0 \0\0\0\34\0\0\0\0\0\0\0Q\0\5\0\14\0\0\0!\0\0\0\34\0\0\0\1\0\0\0Q\0\5\0\14\0\0\0\"\0\0\0\34\0\0\0\2\0\0\0P\0\7\0"
	"\16\0\0\0#\0\0\0 \0\0\0!\0\0\0\"\0\0\0\15\0\0\0\220\0\5\0\16\0\0\0$\0\0\0#\0\0\0\37\0\0\0>\0\3\0\4\0\0\0$\0\0\0>\0\3\0\5\0\0\0\35"
	"\0\0\0>\0\3\0\6\0\0\0\34\0\0\0\375\0\1\0\70\0\1\0";

static const char PS[] =
	"\3\2#\7\0\0\1\0\0\0\16\0|\0\0\0\0\0\0\0\21\0\2\0\1\0\0\0\13\0\6\0\1\0\0\0GLSL.std.450\0\0\0\0\16\0\3\0\0\0\0\0\1\0\0\0\17\0\11\0"
	"\4\0\0\0\2\0\0\0main\0\0\0\0\3\0\0\0\4\0\0\0\5\0\0\0\6\0\0\0\20\0\3\0\2\0\0\0\7\0\0\0\3\0\3\0\5\0\0\0X\2\0\0\5\0\6\0\7\0\0\0type"
	".sampler\0\0\0\0\5\0\6\0\10\0\0\0ColorSampler\0\0\0\0\5\0\7\0\11\0\0\0type.cbWaterConst\0\0\0\6\0\6\0\11\0\0\0\0\0\0\0WaterConst"
	"\0\0\5\0\5\0\12\0\0\0SWaterConst\0\6\0\4\0\12\0\0\0\0\0\0\0mvp\0\6\0\5\0\12\0\0\0\1\0\0\0camPos\0\0\6\0\5\0\12\0\0\0\2\0\0\0pad1"
	"\0\0\0\0\6\0\5\0\12\0\0\0\3\0\0\0boxMax\0\0\6\0\5\0\12\0\0\0\4\0\0\0pad2\0\0\0\0\6\0\5\0\12\0\0\0\5\0\0\0boxMin\0\0\6\0\5\0\12\0"
	"\0\0\6\0\0\0pad3\0\0\0\0\6\0\6\0\12\0\0\0\7\0\0\0cubeCenter\0\0\6\0\5\0\12\0\0\0\10\0\0\0pad4\0\0\0\0\6\0\6\0\12\0\0\0\11\0\0\0b"
	"oxMaxFar\0\0\0\6\0\5\0\12\0\0\0\12\0\0\0pad5\0\0\0\0\6\0\6\0\12\0\0\0\13\0\0\0boxMinFar\0\0\0\6\0\5\0\12\0\0\0\14\0\0\0pad6\0\0\0"
	"\0\5\0\6\0\13\0\0\0cbWaterConst\0\0\0\0\5\0\6\0\14\0\0\0type.2d.image\0\0\0\5\0\4\0\15\0\0\0WaveTex\0\5\0\10\0\16\0\0\0type.cube"
	".image.array\0\0\0\5\0\4\0\17\0\0\0EnvTex\0\0\5\0\7\0\4\0\0\0in.var.TexCoord0\0\0\0\0\5\0\7\0\5\0\0\0in.var.Position0\0\0\0\0\5\0"
	"\7\0\6\0\0\0out.var.SV_TARGET\0\0\0\5\0\4\0\2\0\0\0main\0\0\0\0\5\0\7\0\20\0\0\0type.sampled.image\0\0\5\0\7\0\21\0\0\0type.samp"
	"led.image\0\0G\0\4\0\3\0\0\0\13\0\0\0\17\0\0\0G\0\4\0\4\0\0\0\36\0\0\0\0\0\0\0G\0\4\0\5\0\0\0\36\0\0\0\1\0\0\0G\0\4\0\6\0\0\0\36"
	"\0\0\0\0\0\0\0G\0\4\0\10\0\0\0\"\0\0\0\0\0\0\0G\0\4\0\10\0\0\0!\0\0\0\0\0\0\0G\0\4\0\13\0\0\0\"\0\0\0\1\0\0\0G\0\4\0\13\0\0\0!\0"
	"\0\0\0\0\0\0G\0\4\0\15\0\0\0\"\0\0\0\1\0\0\0G\0\4\0\15\0\0\0!\0\0\0\1\0\0\0G\0\4\0\17\0\0\0\"\0\0\0\1\0\0\0G\0\4\0\17\0\0\0!\0\0"
	"\0\2\0\0\0H\0\5\0\12\0\0\0\0\0\0\0#\0\0\0\0\0\0\0H\0\5\0\12\0\0\0\0\0\0\0\7\0\0\0\20\0\0\0H\0\4\0\12\0\0\0\0\0\0\0\5\0\0\0H\0\5\0"
	"\12\0\0\0\1\0\0\0#\0\0\0@\0\0\0H\0\5\0\12\0\0\0\2\0\0\0#\0\0\0L\0\0\0H\0\5\0\12\0\0\0\3\0\0\0#\0\0\0P\0\0\0H\0\5\0\12\0\0\0\4\0\0"
	"\0#\0\0\0\\\0\0\0H\0\5\0\12\0\0\0\5\0\0\0#\0\0\0`\0\0\0H\0\5\0\12\0\0\0\6\0\0\0#\0\0\0l\0\0\0H\0\5\0\12\0\0\0\7\0\0\0#\0\0\0p\0\0"
	"\0H\0\5\0\12\0\0\0\10\0\0\0#\0\0\0|\0\0\0H\0\5\0\12\0\0\0\11\0\0\0#\0\0\0\200\0\0\0H\0\5\0\12\0\0\0\12\0\0\0#\0\0\0\214\0\0\0H\0"
	"\5\0\12\0\0\0\13\0\0\0#\0\0\0\220\0\0\0H\0\5\0\12\0\0\0\14\0\0\0#\0\0\0\234\0\0\0H\0\5\0\11\0\0\0\0\0\0\0#\0\0\0\0\0\0\0G\0\3\0\11"
	"\0\0\0\2\0\0\0\26\0\3\0\22\0\0\0 \0\0\0+\0\4\0\22\0\0\0\23\0\0\0\0\0\200?+\0\4\0\22\0\0\0\24\0\0\0\0\0\0\0\27\0\4\0\25\0\0\0\22\0"
	"\0\0\4\0\0\0\25\0\4\0\26\0\0\0 \0\0\0\1\0\0\0+\0\4\0\26\0\0\0\27\0\0\0\1\0\0\0+\0\4\0\22\0\0\0\30\0\0\0\0\0\0?,\0\7\0\25\0\0\0\31"
	"\0\0\0\30\0\0\0\30\0\0\0\30\0\0\0\30\0\0\0+\0\4\0\26\0\0\0\32\0\0\0\3\0\0\0+\0\4\0\22\0\0\0\33\0\0\0\12\327#=+\0\4\0\26\0\0\0\34"
	"\0\0\0\0\0\0\0+\0\4\0\22\0\0\0\35\0\0\0\0\0@?+\0\4\0\22\0\0\0\36\0\0\0fff?+\0\4\0\26\0\0\0\37\0\0\0\11\0\0\0+\0\4\0\26\0\0\0 \0\0"
	"\0\13\0\0\0+\0\4\0\26\0\0\0!\0\0\0\5\0\0\0+\0\4\0\26\0\0\0\"\0\0\0\7\0\0\0\32\0\2\0\7\0\0\0 \0\4\0#\0\0\0\0\0\0\0\7\0\0\0\30\0\4"
	"\0$\0\0\0\25\0\0\0\4\0\0\0\27\0\4\0%\0\0\0\22\0\0\0\3\0\0\0\36\0\17\0\12\0\0\0$\0\0\0%\0\0\0\22\0\0\0%\0\0\0\22\0\0\0%\0\0\0\22\0"
	"\0\0%\0\0\0\22\0\0\0%\0\0\0\22\0\0\0%\0\0\0\22\0\0\0\36\0\3\0\11\0\0\0\12\0\0\0 \0\4\0&\0\0\0\2\0\0\0\11\0\0\0\31\0\11\0\14\0\0\0"
	"\22\0\0\0\1\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0 \0\4\0'\0\0\0\0\0\0\0\14\0\0\0\31\0\11\0\16\0\0\0\22\0\0\0\3\0\0\0\2\0"
	"\0\0\1\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0 \0\4\0(\0\0\0\0\0\0\0\16\0\0\0 \0\4\0)\0\0\0\1\0\0\0\25\0\0\0\27\0\4\0*\0\0\0\22\0\0\0\2\0\0"
	"\0 \0\4\0+\0\0\0\1\0\0\0*\0\0\0 \0\4\0,\0\0\0\1\0\0\0%\0\0\0 \0\4\0-\0\0\0\3\0\0\0\25\0\0\0\23\0\2\0.\0\0\0!\0\3\0/\0\0\0.\0\0\0"
	"\33\0\3\0\20\0\0\0\14\0\0\0 \0\4\0\60\0\0\0\2\0\0\0%\0\0\0\33\0\3\0\21\0\0\0\16\0\0\0\24\0\2\0\61\0\0\0;\0\4\0#\0\0\0\10\0\0\0\0"
	"\0\0\0;\0\4\0&\0\0\0\13\0\0\0\2\0\0\0;\0\4\0'\0\0\0\15\0\0\0\0\0\0\0;\0\4\0(\0\0\0\17\0\0\0\0\0\0\0;\0\4\0)\0\0\0\3\0\0\0\1\0\0\0"
	";\0\4\0+\0\0\0\4\0\0\0\1\0\0\0;\0\4\0,\0\0\0\5\0\0\0\1\0\0\0;\0\4\0-\0\0\0\6\0\0\0\3\0\0\0\66\0\5\0.\0\0\0\2\0\0\0\0\0\0\0/\0\0\0"
	"\370\0\2\0\62\0\0\0=\0\4\0*\0\0\0\63\0\0\0\4\0\0\0=\0\4\0%\0\0\0\64\0\0\0\5\0\0\0=\0\4\0\14\0\0\0\65\0\0\0\15\0\0\0=\0\4\0\7\0\0"
	"\0\66\0\0\0\10\0\0\0V\0\5\0\20\0\0\0\67\0\0\0\65\0\0\0\66\0\0\0W\0\6\0\25\0\0\0\70\0\0\0\67\0\0\0\63\0\0\0\0\0\0\0\203\0\5\0\25\0"
	"\0\0\71\0\0\0\70\0\0\0\31\0\0\0Q\0\5\0\22\0\0\0:\0\0\0\71\0\0\0\2\0\0\0Q\0\5\0\22\0\0\0;\0\0\0\71\0\0\0\3\0\0\0P\0\6\0%\0\0\0<\0"
	"\0\0:\0\0\0\33\0\0\0;\0\0\0\14\0\6\0%\0\0\0=\0\0\0\1\0\0\0E\0\0\0<\0\0\0A\0\6\0\60\0\0\0>\0\0\0\13\0\0\0\34\0\0\0\27\0\0\0=\0\4\0"
	"%\0\0\0?\0\0\0>\0\0\0\203\0\5\0%\0\0\0@\0\0\0\64\0\0\0?\0\0\0\14\0\6\0%\0\0\0A\0\0\0\1\0\0\0E\0\0\0@\0\0\0\14\0\7\0%\0\0\0B\0\0\0"
	"\1\0\0\0G\0\0\0A\0\0\0=\0\0\0\14\0\6\0%\0\0\0C\0\0\0\1\0\0\0E\0\0\0B\0\0\0A\0\6\0\60\0\0\0D\0\0\0\13\0\0\0\34\0\0\0\32\0\0\0=\0\4"
	"\0%\0\0\0E\0\0\0D\0\0\0\203\0\5\0%\0\0\0F\0\0\0E\0\0\0\64\0\0\0\210\0\5\0%\0\0\0G\0\0\0F\0\0\0C\0\0\0A\0\6\0\60\0\0\0H\0\0\0\13\0"
	"\0\0\34\0\0\0!\0\0\0=\0\4\0%\0\0\0I\0\0\0H\0\0\0\203\0\5\0%\0\0\0J\0\0\0I\0\0\0\64\0\0\0\210\0\5\0%\0\0\0K\0\0\0J\0\0\0C\0\0\0\14"
	"\0\7\0%\0\0\0L\0\0\0\1\0\0\0(\0\0\0G\0\0\0K\0\0\0Q\0\5\0\22\0\0\0M\0\0\0L\0\0\0\0\0\0\0Q\0\5\0\22\0\0\0N\0\0\0L\0\0\0\1\0\0\0\14"
	"\0\7\0\22\0\0\0O\0\0\0\1\0\0\0%\0\0\0M\0\0\0N\0\0\0Q\0\5\0\22\0\0\0P\0\0\0L\0\0\0\2\0\0\0\14\0\7\0\22\0\0\0Q\0\0\0\1\0\0\0%\0\0\0"
	"O\0\0\0P\0\0\0\216\0\5\0%\0\0\0R\0\0\0C\0\0\0Q\0\0\0\201\0\5\0%\0\0\0S\0\0\0\64\0\0\0R\0\0\0A\0\6\0\60\0\0\0T\0\0\0\13\0\0\0\34\0"
	"\0\0\"\0\0\0=\0\4\0%\0\0\0U\0\0\0T\0\0\0\203\0\5\0%\0\0\0V\0\0\0S\0\0\0U\0\0\0=\0\4\0\16\0\0\0W\0\0\0\17\0\0\0Q\0\5\0\22\0\0\0X\0"
	"\0\0V\0\0\0\0\0\0\0Q\0\5\0\22\0\0\0Y\0\0\0V\0\0\0\1\0\0\0Q\0\5\0\22\0\0\0Z\0\0\0V\0\0\0\2\0\0\0P\0\7\0\25\0\0\0[\0\0\0X\0\0\0Y\0"
	"\0\0Z\0\0\0\24\0\0\0V\0\5\0\21\0\0\0\\\0\0\0W\0\0\0\66\0\0\0W\0\6\0\25\0\0\0]\0\0\0\\\0\0\0[\0\0\0\0\0\0\0Q\0\5\0\22\0\0\0^\0\0\0"
	"]\0\0\0\3\0\0\0\270\0\5\0\61\0\0\0_\0\0\0^\0\0\0\35\0\0\0\367\0\3\0`\0\0\0\0\0\0\0\372\0\4\0_\0\0\0a\0\0\0`\0\0\0\370\0\2\0a\0\0"
	"\0A\0\6\0\60\0\0\0b\0\0\0\13\0\0\0\34\0\0\0\37\0\0\0=\0\4\0%\0\0\0c\0\0\0b\0\0\0\203\0\5\0%\0\0\0d\0\0\0c\0\0\0\64\0\0\0\210\0\5"
	"\0%\0\0\0e\0\0\0d\0\0\0C\0\0\0A\0\6\0\60\0\0\0f\0\0\0\13\0\0\0\34\0\0\0 \0\0\0=\0\4\0%\0\0\0g\0\0\0f\0\0\0\203\0\5\0%\0\0\0h\0\0"
	"\0g\0\0\0\64\0\0\0\210\0\5\0%\0\0\0i\0\0\0h\0\0\0C\0\0\0\14\0\7\0%\0\0\0j\0\0\0\1\0\0\0(\0\0\0e\0\0\0i\0\0\0Q\0\5\0\22\0\0\0k\0\0"
	"\0j\0\0\0\0\0\0\0Q\0\5\0\22\0\0\0l\0\0\0j\0\0\0\1\0\0\0\14\0\7\0\22\0\0\0m\0\0\0\1\0\0\0%\0\0\0k\0\0\0l\0\0\0Q\0\5\0\22\0\0\0n\0"
	"\0\0j\0\0\0\2\0\0\0\14\0\7\0\22\0\0\0o\0\0\0\1\0\0\0%\0\0\0m\0\0\0n\0\0\0\216\0\5\0%\0\0\0p\0\0\0C\0\0\0o\0\0\0\201\0\5\0%\0\0\0"
	"q\0\0\0\64\0\0\0p\0\0\0\203\0\5\0%\0\0\0r\0\0\0q\0\0\0U\0\0\0Q\0\5\0\22\0\0\0s\0\0\0r\0\0\0\0\0\0\0Q\0\5\0\22\0\0\0t\0\0\0r\0\0\0"
	"\1\0\0\0Q\0\5\0\22\0\0\0u\0\0\0r\0\0\0\2\0\0\0P\0\7\0\25\0\0\0v\0\0\0s\0\0\0t\0\0\0u\0\0\0\23\0\0\0V\0\5\0\21\0\0\0w\0\0\0W\0\0\0"
	"\66\0\0\0W\0\6\0\25\0\0\0x\0\0\0w\0\0\0v\0\0\0\0\0\0\0\371\0\2\0`\0\0\0\370\0\2\0`\0\0\0\365\0\7\0\25\0\0\0y\0\0\0]\0\0\0\62\0\0"
	"\0x\0\0\0a\0\0\0\216\0\5\0\25\0\0\0z\0\0\0y\0\0\0\36\0\0\0R\0\6\0\25\0\0\0{\0\0\0\35\0\0\0z\0\0\0\3\0\0\0>\0\3\0\6\0\0\0{\0\0\0\375"
	"\0\1\0\70\0\1\0";

}; // namespace NWater
