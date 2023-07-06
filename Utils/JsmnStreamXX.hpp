/**
 * Original author: Serge Zaitsev <zaitsev.serge@gmail.com>
 * Event based stream parsing rewrite: Sakari Kapanen <sakari.m.kapanen@gmail.com>
 * C++17 conversion: Roberto Duarte <robertodanielalvesduarte@gmail.com>
 */

#pragma once

#include <stddef.h>
#include <yaul.h>

/**
 * JsmnStreamXX
 * MaxDepth Sepecifies the maximum nesting level of the JSON.
 * BufferSize Sepecifies the maximum length a primitive or a string can have.
 */
template <size_t MaxDepth, size_t BufferSize>
struct JsmnStreamXX
{
public:
	enum class StatusResult
	{
		Success,		/* Processed token without errors. */
		Invalid,		/* Invalid character inside JSON string. */
		Incomplete,		/* The string is not a full JSON packet, more bytes expected. */
		BufferOverflow, /* Buffer not large enough. */
		DepthOverflow	/* Reached maximum stack depth (too deep nesting). */
	};

	enum class Action
	{
		ArrayStart,
		ArrayEnd,
		ObjectStart,
		ObjectEnd,
		ObjectKey,
		String,
		Primitive
	};

	StatusResult Parse(char token)
	{
		switch (currentState)
		{
		case State::ParsingString:
			return ParseString(token);

		case State::ParsingPrimitive:
			return ParsePrimitive(token);

		default:
			return ParseToken(token);
		}
	}

	virtual void Process(Action action, const char *string, size_t stringLength) = 0;

private:
	enum class ElementType
	{
		Undefined,
		Object,
		Array,
		String,
		Primitive,
		Key
	};

	enum class State
	{
		ParsingToken,
		ParsingString,
		ParsingPrimitive
	};

	State currentState = State::ParsingToken;
	size_t stackHeight = 0;
	size_t currentBufferPos = 0;

	ElementType typeStack[MaxDepth]; /* Stack for storing the type structure */
	char tokenBuffer[BufferSize] = {};

	template <char... Token>
	constexpr bool FindsMatch(char token)
	{
		return (... || (Token == token));
	}

	bool StackPush(ElementType type)
	{
		if (stackHeight < MaxDepth)
		{
			typeStack[stackHeight++] = type;
			return true;
		}
		else
			return false;
	}

	ElementType StackPop()
	{
		if (stackHeight == 0)
			return ElementType::Undefined;
		else
			return typeStack[stackHeight--];
	}

	ElementType StackTop()
	{
		if (stackHeight == 0)
			return ElementType::Undefined;
		else
			return typeStack[stackHeight - 1];
	}

	StatusResult ParseToken(char token)
	{
		if (FindsMatch<'{', '['>(token))
		{
			ElementType type;
			if (FindsMatch<'{'>(token))
			{
				type = ElementType::Object;
				Process(Action::ObjectStart, nullptr, 0);
			}
			else
			{
				type = ElementType::Array;
				Process(Action::ArrayStart, nullptr, 0);
			}
			if (!StackPush(type))
				return StatusResult::DepthOverflow;
			else
				return StatusResult::Success;
		}

		if (FindsMatch<'}', ']'>(token))
		{
			ElementType type;
			if (FindsMatch<'}'>(token))
				Process(Action::ObjectEnd, nullptr, 0);
			else
				Process(Action::ArrayEnd, nullptr, 0);

			StackPop();
			if (StackTop() == ElementType::Key)
				StackPop();
			return StatusResult::Success;
		}

		if (FindsMatch<'\"'>(token))
		{
			currentState = State::ParsingString;
			return StatusResult::Success;
		}

		if (FindsMatch<'\t', '\r', '\n', ' ', ','>(token))
		{
			return StatusResult::Success;
		}

		if (FindsMatch<':'>(token))
		{
			if (StackTop() == ElementType::Object && !StackPush(ElementType::Key))
				return StatusResult::DepthOverflow;
			else
				return StatusResult::Success;
		}

		if (FindsMatch<'-', 't', 'f', 'n'>(token) || (token >= '0' && token <= '9'))
		{
			if (StackTop() == ElementType::Object)
				return StatusResult::Invalid;

			currentState = State::ParsingPrimitive;
			return ParsePrimitive(token);
		}

		return StatusResult::Invalid;
	}

	bool CheckBufferOverflow()
	{
		/* Leave space for the terminating null character */
		return currentBufferPos == (BufferSize - 1);
	}

	StatusResult ParsePrimitive(char token)
	{
		if (CheckBufferOverflow())
			return StatusResult::BufferOverflow;

		tokenBuffer[currentBufferPos++] = token;
		size_t length = currentBufferPos;
		const char *primitiveBuffer = tokenBuffer;
		for (size_t pos = 0; pos < length && primitiveBuffer[pos] != '\0'; pos++)
		{
			if (FindsMatch<'\t', '\r', '\n', ' ', ',', ']', '}'>(primitiveBuffer[pos]))
			{
				tokenBuffer[length - 1] = '\0';
				Process(Action::Primitive, primitiveBuffer, length - 1);

				currentBufferPos = 0;
				currentState = State::ParsingToken;

				if (StackTop() == ElementType::Key)
					StackPop();

				return ParseToken(token);
			}

			if (primitiveBuffer[pos] < 32 || primitiveBuffer[pos] >= 127)
			{
				return StatusResult::Invalid;
			}
		}
		/* In strict mode primitive must be followed by a comma/object/array */
		return StatusResult::Incomplete;
	}

	StatusResult ParseString(char token)
	{
		if (CheckBufferOverflow())
			return StatusResult::BufferOverflow;

		tokenBuffer[currentBufferPos++] = token;
		size_t length = currentBufferPos;
		const char *stringBuffer = tokenBuffer;
		for (size_t position = 0; position < length; position++)
		{
			/* Quote: end of string */
			if (FindsMatch<'\"'>(stringBuffer[position]))
			{
				tokenBuffer[length - 1] = '\0';
				if (StackTop() == ElementType::Key)
					Process(Action::String, stringBuffer, length - 1);
				else
					Process(Action::ObjectKey, stringBuffer, length - 1);

				currentBufferPos = 0;
				currentState = State::ParsingToken;

				if (StackTop() == ElementType::Key)
					StackPop();
				return StatusResult::Success;
			}

			if (FindsMatch<'\\'>(stringBuffer[position]) && position + 1 < length) /* Backslash: Quoted symbol expected */
			{
				position++;

				if (FindsMatch<'\"', '/', '\\', 'b', 'f', 'r', 'n', 't'>(stringBuffer[position])) /* Allowed escaped symbols */
					break;

				if (FindsMatch<'u'>(stringBuffer[position])) /* Allows escaped symbol \uXXXX */
				{
					position++;
					for (size_t i = 0; i < 4 && position < length && stringBuffer[position] != '\0'; i++)
					{
						/* If it isn't a hex character we have an Invalid StatusResult */
						if (!((stringBuffer[position] >= 48 && stringBuffer[position] <= 57) || /* 0-9 */
							  (stringBuffer[position] >= 65 && stringBuffer[position] <= 70) || /* A-F */
							  (stringBuffer[position] >= 97 && stringBuffer[position] <= 102))) /* a-f */
						{
							return StatusResult::Invalid;
						}
						position++;
					}
					position--;
					break;
				}

				return StatusResult::Invalid; /* Unexpected symbol */
			}
		}
		return StatusResult::Incomplete;
	}
};
