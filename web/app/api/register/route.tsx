import { MongoClient, ObjectId } from "mongodb"
import { NextResponse } from "next/server"

export async function POST(request: Request) {
  try {
    const { username, courseId } = await request.json()

    // Connect to MongoDB
    const uri = "mongodb+srv://public_user:public_password@cluster0.mongodb.net/c-reg_DB"
    const client = new MongoClient(uri)
    await client.connect()

    const db = client.db("c-reg_DB")
    const usersCollection = db.collection("users")
    const coursesCollection = db.collection("courses")

    // Check if course has available capacity
    const course = await coursesCollection.findOne({ _id: new ObjectId(courseId) })

    if (!course) {
      await client.close()
      return NextResponse.json({ success: false, message: "Course not found" }, { status: 404 })
    }

    if (course.enrolled >= course.capacity) {
      await client.close()
      return NextResponse.json({ success: false, message: "Course is full" }, { status: 400 })
    }

    // Update user's registered courses
    await usersCollection.updateOne({ username }, { $addToSet: { registeredCourses: courseId } })

    // Increment course enrollment count
    await coursesCollection.updateOne({ _id: new ObjectId(courseId) }, { $inc: { enrolled: 1 } })

    await client.close()

    return NextResponse.json({ success: true })
  } catch (error) {
    console.error("Registration error:", error)
    return NextResponse.json({ success: false, message: "Server error" }, { status: 500 })
  }
}

export async function DELETE(request: Request) {
  try {
    const { username, courseId } = await request.json()

    // Connect to MongoDB
    const uri = "mongodb+srv://public_user:public_password@cluster0.mongodb.net/c-reg_DB"
    const client = new MongoClient(uri)
    await client.connect()

    const db = client.db("c-reg_DB")
    const usersCollection = db.collection("users")
    const coursesCollection = db.collection("courses")

    // Remove course from user's registered courses
    await usersCollection.updateOne({ username }, { $pull: { registeredCourses: courseId } })

    // Decrement course enrollment count
    await coursesCollection.updateOne({ _id: new ObjectId(courseId) }, { $inc: { enrolled: -1 } })

    await client.close()

    return NextResponse.json({ success: true })
  } catch (error) {
    console.error("Unregistration error:", error)
    return NextResponse.json({ success: false, message: "Server error" }, { status: 500 })
  }
}

