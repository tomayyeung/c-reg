"use client";

import Header from "@/components/Header";
import ChatBot from "@/components/ChatBot";
import { getUserCredit, UserCredit } from "@/util/get-user-credit";
import { getComputerScienceCourse, getMathCourse } from "@/util/get-courses-by-placement";
import { useState } from "react";
import { getCsMajorCourses, getUSFCourseEquivalentsFromAP } from "@/util/get-courses-by-ap";

export default function Recommendations() {
  const [csRecommendation, setCsRecommendation] = useState<string | null>(null);
  const [mathRecommendation, setMathRecommendation] = useState<string | null>(null);

  const handleGetUserCredit = async () => {
    let userCredit: UserCredit | null = null;

    try {
      userCredit = await getUserCredit();
      console.log("userCredit:", userCredit);
    } catch (error) {
      console.error("Failed to fetch user credit:", error);
      return;
    }

    // Check if userCredit is empty
    if (!userCredit) {
      console.log("User credit is empty");
      return;
    }

    // Get the "CS Placement Test" score
    const csPlacementTest = userCredit.placementTests.find(
      (test) => test.testName === "CS Placement Test"
    );
    if (!csPlacementTest) {
      setCsRecommendation("CS Placement Test not found");
      return;
    }
    const csScore = csPlacementTest.testScore;

    // Get the "Math Placement Test" score
    const mathPlacementTest = userCredit.placementTests.find(
      (test) => test.testName === "Math Placement Test"
    );
    if (!mathPlacementTest) {
      setCsRecommendation("Math Placement Test not found");
      return;
    }
    const mathScore = mathPlacementTest.testScore;

    // Get the recommendations from placement tests
    const csMajorCourseRecommendationByPlacement = getComputerScienceCourse(csScore);
    const csMathCourseRecommendationByPlacement = getMathCourse(mathScore);

    // Get AP scores
    const apCredit = userCredit.apCredit;
    const skippableCourses = getUSFCourseEquivalentsFromAP(apCredit);
    const csCourseRecommendationsByAP = getCsMajorCourses(skippableCourses);
    const csMajorCourseRecommendationByAP = csCourseRecommendationsByAP.csMajorCourseRecommendation;
    const csMathCourseRecommendationByAP = csCourseRecommendationsByAP.csMathCourseRecommendation;

    // Choose the higher recommendation between placement and AP
    if (csMajorCourseRecommendationByPlacement > csMajorCourseRecommendationByAP)
      setCsRecommendation(csMajorCourseRecommendationByPlacement);
    else
      setCsRecommendation(csMajorCourseRecommendationByAP);

    if (csMathCourseRecommendationByPlacement > csMathCourseRecommendationByAP)
      setMathRecommendation(csMathCourseRecommendationByPlacement);
    else
      setMathRecommendation(csMathCourseRecommendationByAP);
  };

  /*
    userCredit example:
    {
      "_id": "67f1f85dc07d31826579b559",
      "user": "nish",
      "apCredit": [
        {
          "testName": "AP Computer Science A",
          "testScore": 5
        },
        {
          "testName": "AP Computer Science Principles",
          "testScore": 5
        }
      ],
      "completedCourses": [],
      "placementTests": [
        {
          "testName": "CS Placement Test",
          "testScore": 100
        },
        {
          "testName": "Math Placement Test",
          "testScore": 90
        }
      ]
    }
  */

  return (
    <>
      <Header />
      <ChatBot />

      <button
        onClick={handleGetUserCredit}
        style={{ marginTop: "100px", padding: "10px 20px", cursor: "pointer" }}
      >
        Get User Credit
      </button>

      <br></br>
      <br></br>
      <br></br>
      <br></br>

      {csRecommendation ?? "No recommendation available."}
      <br></br>
      {mathRecommendation ?? "No recommendation available."}
    </>
  );
}