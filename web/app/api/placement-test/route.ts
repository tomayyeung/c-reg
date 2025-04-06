import { NextResponse } from "next/server";
import { MongoClient } from "mongodb";
import dotenv from "dotenv";

dotenv.config();
const uri = process.env.MONGODB_URI;
if (!uri) {
    throw new Error("Env not configured with MONGODB_URI");
}
const client = new MongoClient(uri);

export async function POST(request: Request) {
    try {
        const body = await request.json();
        const { username, placementTestName, placementTestScore } = body;

        // Validate the request data
        if (!username || typeof username !== "string"
            || !placementTestName || typeof placementTestName !== "string"
            || typeof placementTestScore !== "number" || placementTestScore < 0 || placementTestScore > 100) {
            return NextResponse.json({ error: "Invalid request data" }, { status: 400 });
        }

        await client.connect();
        const database = client.db("c-reg_DB");
        const creditCollection = database.collection("credit");

        // Find the user in the collection
        const user = await creditCollection.findOne({ user: username });

        if (user) {
            // If user exists, update the placementTests array (add the placement test object without duplicates)
            await creditCollection.updateOne(
                { user: username },
                { $addToSet: { placementTests: { testName: placementTestName, testScore: placementTestScore } } },    // Add unique placement test object
            );
        } else {
            // If user doesn't exist, create a new document
            await creditCollection.insertOne({
                user: username,
                apCredit: [],
                completedCourses: [],
                placementTests: [{ testName: placementTestName, testScore: placementTestScore }], // Add the placement test object
            });
        }

        return NextResponse.json({ success: true, message: "Successfully added placement test to the account." }, { status: 201 });
    } catch (error) {
        console.error("Error adding placement test:", error);
        return NextResponse.json({ error: "Failed to add placement test" }, { status: 500 });
    } finally {
        // Close the connection when done
        await client.close();
    }
}