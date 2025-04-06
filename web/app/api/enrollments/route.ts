import { NextResponse } from "next/server"
import { MongoClient, ObjectId } from "mongodb"
import dotenv from "dotenv";
// MongoDB connection string should be in your environment variables
dotenv.config();
const uri = process.env.MONGODB_URI
if (!uri) {
    throw new Error("Env not configured with MONGODB_URI");
  }
const client = new MongoClient(uri)


export async function POST(request: Request) {
  try {
    const body = await request.json()
    const { username, sections } = body

    if (!username || !sections || !Array.isArray(sections)) {
      return NextResponse.json({ error: "Invalid request data" }, { status: 400 })
    }

    await client.connect()
    const database = client.db("c-reg_DB")
    const enrollmentsCollection = database.collection("enrollments")
    const sectionsCollection = database.collection("sections")

    // Convert section IDs to ObjectId
    const sectionObjectIds = sections.map((id) => new ObjectId(id))

    // Get section details
    const sectionDetails = await sectionsCollection.find({ _id: { $in: sectionObjectIds } }).toArray()

    // Check if any section is at capacity
    const atCapacitySections = sectionDetails.filter((section) => section.enrollment >= section.capacity)

    if (atCapacitySections.length > 0) {
      return NextResponse.json(
        {
          error: "One or more sections are at capacity",
          sections: atCapacitySections.map((s) => s.course),
        },
        { status: 400 },
      )
    }

    // Create enrollment records
    const enrollmentData = sections.map((sectionId) => ({
      username,
      section_id: new ObjectId(sectionId),
      enrolled_date: new Date(),
    }))

    // Insert enrollments
    await enrollmentsCollection.insertMany(enrollmentData)

    // Update section enrollment counts
    for (const sectionId of sectionObjectIds) {
      await sectionsCollection.updateOne({ _id: sectionId }, { $inc: { enrollment: 1 } })
    }

    return NextResponse.json({ success: true, message: "Successfully enrolled in classes" }, { status: 201 })
  } catch (error) {
    console.error("Error creating enrollments:", error)
    return NextResponse.json({ error: "Failed to create enrollments" }, { status: 500 })
  } finally {
    // Close the connection when done
    await client.close()
  }
}

export async function GET(request: Request) {
  try {
    const url = new URL(request.url)
    const username = url.searchParams.get("username")

    if (!username) {
      return NextResponse.json({ error: "Username is required" }, { status: 400 })
    }

    await client.connect()
    const database = client.db("c-reg_DB")
    const enrollmentsCollection = database.collection("enrollments")
    const sectionsCollection = database.collection("sections")

    // Get user's enrollments
    const enrollments = await enrollmentsCollection.find({ username }).toArray()

    // Get section details for each enrollment
    const sectionIds = enrollments.map((e) => e.section_id)
    const sections = await sectionsCollection.find({ _id: { $in: sectionIds } }).toArray()

    // Combine enrollment data with section details
    const enrollmentDetails = enrollments.map((enrollment) => {
      const section = sections.find((s) => s._id.toString() === enrollment.section_id.toString())
      return {
        ...enrollment,
        section,
      }
    })

    return NextResponse.json(enrollmentDetails, { status: 200 })
  } catch (error) {
    console.error("Error fetching enrollments:", error)
    return NextResponse.json({ error: "Failed to fetch enrollments" }, { status: 500 })
  } finally {
    // Close the connection when done
    await client.close()
  }
}

