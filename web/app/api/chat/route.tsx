import OpenAI from "openai";
import { NextResponse } from "next/server";
import dotenv from "dotenv";
dotenv.config();

// Allow streaming responses up to 30 seconds
export const maxDuration = 30;

const client = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY,
});

export async function POST(req: Request) {
  try {
    // Parse the request body
    const { messages } = await req.json();

    // Validate input
    if (!messages || !Array.isArray(messages)) {
      return NextResponse.json(
        { error: "Invalid request: messages array required" },
        { status: 400 }
      );
    }

    console.log("User message:", messages[messages.length - 1]?.content);

    // Use the non-streaming approach to get the response
    const completion = await client.chat.completions.create({
      model: "gpt-4o",
      messages: messages,
    });

    // Create a response with the assistant's message
    const responseMessage = {
      id: crypto.randomUUID(),
      role: 'assistant',
      content: completion.choices[0].message.content || '',
    };

    // Return a standard JSON response
    return NextResponse.json(responseMessage);

  } catch (error) {
    console.error("Error in chat API route:", error);
    return NextResponse.json(
      { error: "Internal server error" },
      { status: 500 }
    );
  }
}