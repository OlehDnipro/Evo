#pragma once

namespace NShadowMapCascade {


enum Root
{
	CB,
	Resources,
	Samplers,
};

enum NCB
{
	perFrame,
	perModel,

};

enum NResources
{
	Texture,

};

static const char RootSig[] =
	"\3\0\0\0\7\0\0\0\2\0\0\0\7\0\0\0\1\0\0\0\10\0\0\0\1\0\0\0\5\0\0\0\1\0\0\0\5\0\0\0\1\0\0\0\1\0\0\0\1\0\0\0\6\0\0\0\1\0\0\0";

static const char VertexShader[] =
	"\3\2#\7\0\0\1\0\0\0\16\0A\0\0\0\0\0\0\0\21\0\2\0\1\0\0\0\16\0\3\0\0\0\0\0\1\0\0\0\17\0\17\0\0\0\0\0\1\0\0\0main\0\0\0\0\2\0\0\0\3"
	"\0\0\0\4\0\0\0\5\0\0\0\6\0\0\0\7\0\0\0\10\0\0\0\11\0\0\0\12\0\0\0\13\0\0\0\3\0\3\0\5\0\0\0X\2\0\0\5\0\6\0\14\0\0\0type.cbperFram"
	"e\0\6\0\6\0\14\0\0\0\0\0\0\0perFrame\0\0\0\0\5\0\5\0\15\0\0\0SPerFrame\0\0\0\6\0\6\0\15\0\0\0\0\0\0\0projection\0\0\6\0\5\0\15\0"
	"\0\0\1\0\0\0view\0\0\0\0\6\0\6\0\15\0\0\0\2\0\0\0lightDir\0\0\0\0\5\0\5\0\16\0\0\0cbperFrame\0\0\5\0\6\0\17\0\0\0type.cbperModel"
	"\0\6\0\6\0\17\0\0\0\0\0\0\0perModel\0\0\0\0\5\0\5\0\20\0\0\0SPerModel\0\0\0\6\0\5\0\20\0\0\0\0\0\0\0model\0\0\0\5\0\5\0\21\0\0\0"
	"cbperModel\0\0\5\0\6\0\2\0\0\0in.var.Position\0\5\0\6\0\3\0\0\0in.var.TexCoord\0\5\0\6\0\4\0\0\0in.var.Color\0\0\0\0\5\0\6\0\5\0"
	"\0\0in.var.Normal\0\0\0\5\0\7\0\7\0\0\0out.var.Position0\0\0\0\5\0\7\0\10\0\0\0out.var.Position1\0\0\0\5\0\7\0\11\0\0\0out.var.T"
	"exCoord\0\0\0\0\5\0\6\0\12\0\0\0out.var.Color\0\0\0\5\0\6\0\13\0\0\0out.var.Normal\0\0\5\0\4\0\1\0\0\0main\0\0\0\0G\0\4\0\6\0\0\0"
	"\13\0\0\0\0\0\0\0G\0\4\0\2\0\0\0\36\0\0\0\0\0\0\0G\0\4\0\3\0\0\0\36\0\0\0\1\0\0\0G\0\4\0\4\0\0\0\36\0\0\0\2\0\0\0G\0\4\0\5\0\0\0"
	"\36\0\0\0\3\0\0\0G\0\4\0\7\0\0\0\36\0\0\0\0\0\0\0G\0\4\0\10\0\0\0\36\0\0\0\1\0\0\0G\0\4\0\11\0\0\0\36\0\0\0\2\0\0\0G\0\4\0\12\0\0"
	"\0\36\0\0\0\3\0\0\0G\0\4\0\13\0\0\0\36\0\0\0\4\0\0\0G\0\4\0\16\0\0\0\"\0\0\0\0\0\0\0G\0\4\0\16\0\0\0!\0\0\0\0\0\0\0G\0\4\0\21\0\0"
	"\0\"\0\0\0\0\0\0\0G\0\4\0\21\0\0\0!\0\0\0\1\0\0\0H\0\5\0\15\0\0\0\0\0\0\0#\0\0\0\0\0\0\0H\0\5\0\15\0\0\0\0\0\0\0\7\0\0\0\20\0\0\0"
	"H\0\4\0\15\0\0\0\0\0\0\0\4\0\0\0H\0\5\0\15\0\0\0\1\0\0\0#\0\0\0@\0\0\0H\0\5\0\15\0\0\0\1\0\0\0\7\0\0\0\20\0\0\0H\0\4\0\15\0\0\0\1"
	"\0\0\0\4\0\0\0H\0\5\0\15\0\0\0\2\0\0\0#\0\0\0\200\0\0\0H\0\5\0\14\0\0\0\0\0\0\0#\0\0\0\0\0\0\0G\0\3\0\14\0\0\0\2\0\0\0H\0\5\0\20"
	"\0\0\0\0\0\0\0#\0\0\0\0\0\0\0H\0\5\0\20\0\0\0\0\0\0\0\7\0\0\0\20\0\0\0H\0\4\0\20\0\0\0\0\0\0\0\4\0\0\0H\0\5\0\17\0\0\0\0\0\0\0#\0"
	"\0\0\0\0\0\0G\0\3\0\17\0\0\0\2\0\0\0\25\0\4\0\22\0\0\0 \0\0\0\1\0\0\0+\0\4\0\22\0\0\0\23\0\0\0\1\0\0\0+\0\4\0\22\0\0\0\24\0\0\0\0"
	"\0\0\0\26\0\3\0\25\0\0\0 \0\0\0+\0\4\0\25\0\0\0\26\0\0\0\0\0\200?\27\0\4\0\27\0\0\0\25\0\0\0\4\0\0\0\30\0\4\0\30\0\0\0\27\0\0\0\4"
	"\0\0\0\27\0\4\0\31\0\0\0\25\0\0\0\3\0\0\0\36\0\5\0\15\0\0\0\30\0\0\0\30\0\0\0\31\0\0\0\36\0\3\0\14\0\0\0\15\0\0\0 \0\4\0\32\0\0\0"
	"\2\0\0\0\14\0\0\0\36\0\3\0\20\0\0\0\30\0\0\0\36\0\3\0\17\0\0\0\20\0\0\0 \0\4\0\33\0\0\0\2\0\0\0\17\0\0\0 \0\4\0\34\0\0\0\1\0\0\0"
	"\31\0\0\0\27\0\4\0\35\0\0\0\25\0\0\0\2\0\0\0 \0\4\0\36\0\0\0\1\0\0\0\35\0\0\0 \0\4\0\37\0\0\0\3\0\0\0\27\0\0\0 \0\4\0 \0\0\0\3\0"
	"\0\0\31\0\0\0 \0\4\0!\0\0\0\3\0\0\0\35\0\0\0\23\0\2\0\"\0\0\0!\0\3\0#\0\0\0\"\0\0\0 \0\4\0$\0\0\0\2\0\0\0\30\0\0\0;\0\4\0\32\0\0"
	"\0\16\0\0\0\2\0\0\0;\0\4\0\33\0\0\0\21\0\0\0\2\0\0\0;\0\4\0\34\0\0\0\2\0\0\0\1\0\0\0;\0\4\0\36\0\0\0\3\0\0\0\1\0\0\0;\0\4\0\34\0"
	"\0\0\4\0\0\0\1\0\0\0;\0\4\0\34\0\0\0\5\0\0\0\1\0\0\0;\0\4\0\37\0\0\0\6\0\0\0\3\0\0\0;\0\4\0 \0\0\0\7\0\0\0\3\0\0\0;\0\4\0 \0\0\0"
	"\10\0\0\0\3\0\0\0;\0\4\0!\0\0\0\11\0\0\0\3\0\0\0;\0\4\0 \0\0\0\12\0\0\0\3\0\0\0;\0\4\0 \0\0\0\13\0\0\0\3\0\0\0\66\0\5\0\"\0\0\0\1"
	"\0\0\0\0\0\0\0#\0\0\0\370\0\2\0%\0\0\0=\0\4\0\31\0\0\0&\0\0\0\2\0\0\0=\0\4\0\35\0\0\0'\0\0\0\3\0\0\0=\0\4\0\31\0\0\0(\0\0\0\4\0\0"
	"\0=\0\4\0\31\0\0\0)\0\0\0\5\0\0\0Q\0\5\0\25\0\0\0*\0\0\0&\0\0\0\0\0\0\0Q\0\5\0\25\0\0\0+\0\0\0&\0\0\0\1\0\0\0Q\0\5\0\25\0\0\0,\0"
	"\0\0&\0\0\0\2\0\0\0P\0\7\0\27\0\0\0-\0\0\0*\0\0\0+\0\0\0,\0\0\0\26\0\0\0A\0\6\0$\0\0\0.\0\0\0\21\0\0\0\24\0\0\0\24\0\0\0=\0\4\0\30"
	"\0\0\0/\0\0\0.\0\0\0\221\0\5\0\27\0\0\0\60\0\0\0/\0\0\0-\0\0\0O\0\10\0\31\0\0\0\61\0\0\0\60\0\0\0\60\0\0\0\0\0\0\0\1\0\0\0\2\0\0"
	"\0Q\0\5\0\25\0\0\0\62\0\0\0\60\0\0\0\0\0\0\0Q\0\5\0\25\0\0\0\63\0\0\0\60\0\0\0\1\0\0\0Q\0\5\0\25\0\0\0\64\0\0\0\60\0\0\0\2\0\0\0"
	"P\0\7\0\27\0\0\0\65\0\0\0\62\0\0\0\63\0\0\0\64\0\0\0\26\0\0\0A\0\6\0$\0\0\0\66\0\0\0\16\0\0\0\24\0\0\0\23\0\0\0=\0\4\0\30\0\0\0\67"
	"\0\0\0\66\0\0\0\221\0\5\0\27\0\0\0\70\0\0\0\67\0\0\0\65\0\0\0O\0\10\0\31\0\0\0\71\0\0\0\70\0\0\0\70\0\0\0\0\0\0\0\1\0\0\0\2\0\0\0"
	"Q\0\5\0\25\0\0\0:\0\0\0\70\0\0\0\0\0\0\0Q\0\5\0\25\0\0\0;\0\0\0\70\0\0\0\1\0\0\0Q\0\5\0\25\0\0\0<\0\0\0\70\0\0\0\2\0\0\0P\0\7\0\27"
	"\0\0\0=\0\0\0:\0\0\0;\0\0\0<\0\0\0\26\0\0\0A\0\6\0$\0\0\0>\0\0\0\16\0\0\0\24\0\0\0\24\0\0\0=\0\4\0\30\0\0\0?\0\0\0>\0\0\0\221\0\5"
	"\0\27\0\0\0@\0\0\0?\0\0\0=\0\0\0>\0\3\0\6\0\0\0@\0\0\0>\0\3\0\7\0\0\0\61\0\0\0>\0\3\0\10\0\0\0\71\0\0\0>\0\3\0\11\0\0\0'\0\0\0>\0"
	"\3\0\12\0\0\0(\0\0\0>\0\3\0\13\0\0\0)\0\0\0\375\0\1\0\70\0\1\0";

static const char PixelShader[] =
	"\3\2#\7\0\0\1\0\0\0\16\0A\0\0\0\0\0\0\0\21\0\2\0\1\0\0\0\13\0\6\0\1\0\0\0GLSL.std.450\0\0\0\0\16\0\3\0\0\0\0\0\1\0\0\0\17\0\14\0"
	"\4\0\0\0\2\0\0\0main\0\0\0\0\3\0\0\0\4\0\0\0\5\0\0\0\6\0\0\0\7\0\0\0\10\0\0\0\11\0\0\0\20\0\3\0\2\0\0\0\7\0\0\0\3\0\3\0\5\0\0\0X"
	"\2\0\0\5\0\6\0\12\0\0\0type.cbperFrame\0\6\0\6\0\12\0\0\0\0\0\0\0perFrame\0\0\0\0\5\0\5\0\13\0\0\0SPerFrame\0\0\0\6\0\6\0\13\0\0"
	"\0\0\0\0\0projection\0\0\6\0\5\0\13\0\0\0\1\0\0\0view\0\0\0\0\6\0\6\0\13\0\0\0\2\0\0\0lightDir\0\0\0\0\5\0\5\0\14\0\0\0cbperFram"
	"e\0\0\5\0\6\0\15\0\0\0type.2d.image\0\0\0\5\0\4\0\16\0\0\0Texture\0\5\0\6\0\17\0\0\0type.sampler\0\0\0\0\5\0\4\0\20\0\0\0Filter\0"
	"\0\5\0\7\0\4\0\0\0in.var.Position0\0\0\0\0\5\0\7\0\5\0\0\0in.var.Position1\0\0\0\0\5\0\6\0\6\0\0\0in.var.TexCoord\0\5\0\6\0\7\0\0"
	"\0in.var.Color\0\0\0\0\5\0\6\0\10\0\0\0in.var.Normal\0\0\0\5\0\7\0\11\0\0\0out.var.SV_TARGET\0\0\0\5\0\4\0\2\0\0\0main\0\0\0\0\5"
	"\0\7\0\21\0\0\0type.sampled.image\0\0G\0\4\0\3\0\0\0\13\0\0\0\17\0\0\0G\0\4\0\4\0\0\0\36\0\0\0\0\0\0\0G\0\4\0\5\0\0\0\36\0\0\0\1"
	"\0\0\0G\0\4\0\6\0\0\0\36\0\0\0\2\0\0\0G\0\4\0\7\0\0\0\36\0\0\0\3\0\0\0G\0\4\0\10\0\0\0\36\0\0\0\4\0\0\0G\0\4\0\11\0\0\0\36\0\0\0"
	"\0\0\0\0G\0\4\0\14\0\0\0\"\0\0\0\0\0\0\0G\0\4\0\14\0\0\0!\0\0\0\0\0\0\0G\0\4\0\16\0\0\0\"\0\0\0\1\0\0\0G\0\4\0\16\0\0\0!\0\0\0\0"
	"\0\0\0G\0\4\0\20\0\0\0\"\0\0\0\2\0\0\0G\0\4\0\20\0\0\0!\0\0\0\0\0\0\0H\0\5\0\13\0\0\0\0\0\0\0#\0\0\0\0\0\0\0H\0\5\0\13\0\0\0\0\0"
	"\0\0\7\0\0\0\20\0\0\0H\0\4\0\13\0\0\0\0\0\0\0\4\0\0\0H\0\5\0\13\0\0\0\1\0\0\0#\0\0\0@\0\0\0H\0\5\0\13\0\0\0\1\0\0\0\7\0\0\0\20\0"
	"\0\0H\0\4\0\13\0\0\0\1\0\0\0\4\0\0\0H\0\5\0\13\0\0\0\2\0\0\0#\0\0\0\200\0\0\0H\0\5\0\12\0\0\0\0\0\0\0#\0\0\0\0\0\0\0G\0\3\0\12\0"
	"\0\0\2\0\0\0\26\0\3\0\22\0\0\0 \0\0\0\25\0\4\0\23\0\0\0 \0\0\0\1\0\0\0+\0\4\0\22\0\0\0\24\0\0\0\0\0\0?+\0\4\0\22\0\0\0\25\0\0\0\0"
	"\0\0\0+\0\4\0\23\0\0\0\26\0\0\0\2\0\0\0+\0\4\0\23\0\0\0\27\0\0\0\0\0\0\0+\0\4\0\22\0\0\0\30\0\0\0\232\231\231>\27\0\4\0\31\0\0\0"
	"\22\0\0\0\3\0\0\0,\0\6\0\31\0\0\0\32\0\0\0\25\0\0\0\25\0\0\0\25\0\0\0\27\0\4\0\33\0\0\0\22\0\0\0\4\0\0\0\30\0\4\0\34\0\0\0\33\0\0"
	"\0\4\0\0\0\36\0\5\0\13\0\0\0\34\0\0\0\34\0\0\0\31\0\0\0\36\0\3\0\12\0\0\0\13\0\0\0 \0\4\0\35\0\0\0\2\0\0\0\12\0\0\0\31\0\11\0\15"
	"\0\0\0\22\0\0\0\1\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\1\0\0\0\0\0\0\0 \0\4\0\36\0\0\0\0\0\0\0\15\0\0\0\32\0\2\0\17\0\0\0 \0\4\0\37\0\0"
	"\0\0\0\0\0\17\0\0\0 \0\4\0 \0\0\0\1\0\0\0\33\0\0\0 \0\4\0!\0\0\0\1\0\0\0\31\0\0\0\27\0\4\0\"\0\0\0\22\0\0\0\2\0\0\0 \0\4\0#\0\0\0"
	"\1\0\0\0\"\0\0\0 \0\4\0$\0\0\0\3\0\0\0\33\0\0\0\23\0\2\0%\0\0\0!\0\3\0&\0\0\0%\0\0\0\33\0\3\0\21\0\0\0\15\0\0\0\24\0\2\0'\0\0\0 "
	"\0\4\0(\0\0\0\2\0\0\0\31\0\0\0;\0\4\0\35\0\0\0\14\0\0\0\2\0\0\0;\0\4\0\36\0\0\0\16\0\0\0\0\0\0\0;\0\4\0\37\0\0\0\20\0\0\0\0\0\0\0"
	";\0\4\0 \0\0\0\3\0\0\0\1\0\0\0;\0\4\0!\0\0\0\4\0\0\0\1\0\0\0;\0\4\0!\0\0\0\5\0\0\0\1\0\0\0;\0\4\0#\0\0\0\6\0\0\0\1\0\0\0;\0\4\0!"
	"\0\0\0\7\0\0\0\1\0\0\0;\0\4\0!\0\0\0\10\0\0\0\1\0\0\0;\0\4\0$\0\0\0\11\0\0\0\3\0\0\0\1\0\3\0\33\0\0\0)\0\0\0\66\0\5\0%\0\0\0\2\0"
	"\0\0\0\0\0\0&\0\0\0\370\0\2\0*\0\0\0=\0\4\0\"\0\0\0+\0\0\0\6\0\0\0=\0\4\0\31\0\0\0,\0\0\0\10\0\0\0=\0\4\0\15\0\0\0-\0\0\0\16\0\0"
	"\0=\0\4\0\17\0\0\0.\0\0\0\20\0\0\0V\0\5\0\21\0\0\0/\0\0\0-\0\0\0.\0\0\0W\0\6\0\33\0\0\0\60\0\0\0/\0\0\0+\0\0\0\0\0\0\0Q\0\5\0\22"
	"\0\0\0\61\0\0\0\60\0\0\0\3\0\0\0\270\0\5\0'\0\0\0\62\0\0\0\61\0\0\0\24\0\0\0\367\0\3\0\63\0\0\0\0\0\0\0\372\0\4\0\62\0\0\0\64\0\0"
	"\0\63\0\0\0\370\0\2\0\64\0\0\0\374\0\1\0\370\0\2\0\63\0\0\0\14\0\6\0\31\0\0\0\65\0\0\0\1\0\0\0E\0\0\0,\0\0\0A\0\6\0(\0\0\0\66\0\0"
	"\0\14\0\0\0\27\0\0\0\26\0\0\0=\0\4\0\31\0\0\0\67\0\0\0\66\0\0\0\177\0\4\0\31\0\0\0\70\0\0\0\67\0\0\0\14\0\6\0\31\0\0\0\71\0\0\0\1"
	"\0\0\0E\0\0\0\70\0\0\0\224\0\5\0\22\0\0\0:\0\0\0\65\0\0\0\71\0\0\0\14\0\7\0\22\0\0\0;\0\0\0\1\0\0\0(\0\0\0:\0\0\0\30\0\0\0O\0\10"
	"\0\31\0\0\0<\0\0\0\60\0\0\0\60\0\0\0\0\0\0\0\1\0\0\0\2\0\0\0\216\0\5\0\31\0\0\0=\0\0\0<\0\0\0;\0\0\0\14\0\7\0\31\0\0\0>\0\0\0\1\0"
	"\0\0(\0\0\0=\0\0\0\32\0\0\0O\0\11\0\33\0\0\0?\0\0\0)\0\0\0>\0\0\0\4\0\0\0\5\0\0\0\6\0\0\0\3\0\0\0R\0\6\0\33\0\0\0@\0\0\0\61\0\0\0"
	"?\0\0\0\3\0\0\0>\0\3\0\11\0\0\0@\0\0\0\375\0\1\0\70\0\1\0";

}; // namespace NShadowMapCascade