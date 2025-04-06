import { NextResponse } from "next/server";
import { MongoClient } from "mongodb";
import dotenv from "dotenv";

dotenv.config();
const uri = process.env.MONGODB_URI;
if (!uri) {
    throw new Error("Env not configured with MONGODB_URI");
}
const client = new MongoClient(uri);

/*
  Credit Collection Document Format:
  {
    _id: ObjectId,
    user: String,
    apCredit: Array of Objects (each object is { testName: String, testScore: Number }),
    completedCourses: Array of Numbers
  }
*/

export async function POST(request: Request) {
    try {
        const body = await request.json();
        const { username, apTestName, apTestScore } = body;

        // Validate the request data
        if (!username || typeof username !== "string"
            || !apTestName || typeof apTestName !== "string"
            || !apTestScore || typeof apTestScore !== "number" || apTestScore < 1 || apTestScore > 5) {
            return NextResponse.json({ error: "Invalid request data" }, { status: 400 });
        }

        await client.connect();
        const database = client.db("c-reg_DB");
        const creditCollection = database.collection("credit");

        // Find the user in the collection
        const user = await creditCollection.findOne({ user: username });

        if (user) {
            // If user exists, update the apCredit array (add the AP test object without duplicates)
            await creditCollection.updateOne(
                { user: username },
                { $addToSet: { apCredit: { testName: apTestName, testScore: apTestScore }, }, },    // Add unique AP test object
            );
        } else {
            // If user doesn't exist, create a new document
            await creditCollection.insertOne({
                user: username,
                apCredit: [{ testName: apTestName, testScore: apTestScore }], // Add the AP test object
                completedCourses: [],
            });
        }

        return NextResponse.json({ success: true, message: "Successfully added AP credit to the account." }, { status: 201 });
    } catch (error) {
        console.error("Error adding AP credit:", error);
        return NextResponse.json({ error: "Failed to add AP credit" }, { status: 500 });
    } finally {
        // Close the connection when done
        await client.close();
    }
}