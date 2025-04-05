import { MongoClient } from "mongodb"
import { NextResponse } from "next/server"

export async function POST(request: Request) {
  try {
    const { username, password } = await request.json()

    // Connect to MongoDB
    const uri = "mongodb+srv://leozhang2024:bzZhMIdXY8hDYXxe@course-reg-cluster.pyj2eqo.mongodb.net/?retryWrites=true&w=majority&appName=course-reg-cluster"
    const client = new MongoClient(uri)
    await client.connect()

    const db = client.db("c-reg_DB")
    const usersCollection = db.collection("users")

    // Find user with matching username and password

    const user = await usersCollection.findOne({ name: username, password });
    

    await client.close()

    if (user) {
      return NextResponse.json({ success: true, username })
    } else {
      return NextResponse.json({ success: false, message: "Invalid username or password" }, { status: 401 })
    }
  } catch (error) {
    console.error("Authentication error:", error)
    return NextResponse.json({ success: false, message: "Server error" }, { status: 500 })
  }
}

