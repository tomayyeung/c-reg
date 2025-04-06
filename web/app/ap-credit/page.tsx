"use client";

import { useState, useEffect } from "react";
import Header from "@/components/Header";
import ChatBot from "@/components/ChatBot";
import styles from "./page.module.css";

type ApTestCreditMap = {
  [key: string]: {
    score: number;
    credit: string;
  }
};

const apTestCreditMap: ApTestCreditMap = {
  "AP African American Studies": { score: 3, credit: "General Elective" },
  "AP Art History": { score: 3, credit: "Visual and Performing Arts Core" },
  "AP Art: Studio Drawing": { score: 3, credit: "General Elective" },
  "AP Art: Studio General": { score: 3, credit: "General Elective" },
  "AP Art: 2D Design": { score: 3, credit: "General Elective" },
  "AP Art: 3D Design": { score: 3, credit: "General Elective" },
  "AP Biology": { score: 4, credit: "BIOL 100 and 103" },
  "AP Chemistry": { score: 4, credit: "CHEM 111 and 113" },
  "AP Chinese Language and Culture": { score: 3, credit: "Reach out to department to determine level." },
  "AP Computer Science A": { score: 3, credit: "CS 110" },
  "AP Computer Science AB": { score: 3, credit: "CS 110 and 112" },
  "AP Computer Science Principles": { score: 4, credit: "CS 107" },
  "AP Economics: Micro": { score: 3, credit: "ECON 111" },
  "AP Economics: Macro": { score: 3, credit: "ECON 112" },
  "AP English Language and Composition": { score: 4, credit: "General Elective" },
  "AP English Literature and Composition": { score: 3, credit: "Literature Core" },
  "AP Environmental Science": { score: 3, credit: "Science Core" },
  "AP European History": { score: 4, credit: "HIST 110" },
  "AP French Language": { score: 3, credit: "Reach out to department to determine level." },
  "AP Government & Politics: U.S.": { score: 3, credit: "Reach out to department to determine level." },
  "AP Government & Politics: Comparative": { score: 3, credit: "Reach out to department to determine level." },
  "AP German Language": { score: 3, credit: "Reach out to department to determine level." },
  "AP Human Geography": { score: 3, credit: "General Elective" },
  "AP Italian Language and Culture": { score: 3, credit: "Reach out to department to determine level." },
  "AP Japanese Language and Culture": { score: 3, credit: "Reach out to department to determine level." },
  "AP Latin": { score: 3, credit: "Reach out to department to determine level." },
  "AP Calculus AB": { score: 4, credit: "MATH 109" },
  "AP Calculus BC": { score: 4, credit: "MATH 109 and MATH 110" },
  "AP Calculus AB Subgrade": { score: 4, credit: "MATH 109" },
  "AP Music: Listening and Literature": { score: 3, credit: "General Elective" },
  "AP Music: Theory": { score: 3, credit: "General Elective" },
  "AP Physics B": { score: 3, credit: "PHYS 100 and PHYS 101" },
  "AP Physics C: Mechanics": { score: 3, credit: "PHYS 110" },
  "AP Physics C: Electricity and Magnetism": { score: 3, credit: "PHYS 210" },
  "AP Physics 1": { score: 3, credit: "PHYS 100" },
  "AP Physics 2": { score: 3, credit: "PHYS 101" },
  "AP Psychology": { score: 4, credit: "PSYC 101" },
  "AP Spanish Language": { score: 3, credit: "Reach out to department to determine level." },
  "AP Spanish Literature": { score: 3, credit: "Core Literature" },
  "AP Statistics": { score: 3, credit: "MATH 101" },
  "AP US History": { score: 4, credit: "US History Core and HIST 120" },
  "AP World History": { score: 4, credit: "General Elective" }
};

const apTests = Object.keys(apTestCreditMap);

export default function APCredit() {
  const [testName, setTestName] = useState<string>(apTests[0]);
  const [testScore, setTestScore] = useState<number>(1);
  const [earnedCredit, setEarnedCredit] = useState<string | null>(null);


  useEffect(() => {
    if (!testName || testScore < 1 || testScore > 5) {
      setEarnedCredit(null);
      return;
    }

    const testCredit = apTestCreditMap[testName];
    if (!testCredit) {
      setEarnedCredit(null);
      return;
    }

    if (testScore < testCredit.score) {
      setEarnedCredit("No credit, need a score of " + testCredit.score + " or higher");
      return;
    }

    if (testCredit.credit === "General Elective") {
      setEarnedCredit("This counts as a General Elective");
      return;
    }

    if (testName === "AP Biology" && testScore == 5) {
      setEarnedCredit("This counts as " + testCredit.credit + ". You could also petition the Biology Chair to receive credit for BIOL 105-General Biology I (4 credits) and BIOL 106-General Biology II (4 credits) in place of credit for BIOL 100 and BIOL 103");
    } else {
      setEarnedCredit("This counts as " + testCredit.credit);
    }
  }, [testName, testScore]);


  const handleTestChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    setTestName(e.target.value);
  };


  const handleAddTestToAccount = async () => {
    if (!testName || testScore < 1 || testScore > 5) {
      alert("Please select a valid test and score.");
      return;
    }

    const username = localStorage.getItem("currentUser");
    if (!username) {
      alert("Please log in first.");
      return;
    }

    try {
      const response = await fetch("/api/ap-credit", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          username,
          apTestName: testName,
          apTestScore: testScore
        }),
      });

      if (!response.ok) {
        alert("Failed to add AP Test. Please try again.");
      }
    } catch (error) {
      console.error("Error adding AP Test:", error);
      alert("An error occurred. Please try again.");
    }
  };


  return (
    <>
      <Header />
      <ChatBot />

      <div className={styles.container}>
        <div className={styles.main}>
          <div className={styles.hero}>
            <h1 className={styles.title}>AP Test Credit</h1>
            <p className={styles.subtitle}>
              Select your AP test and enter your score to see how much credit you earned.
            </p>
          </div>

          <div className={styles.form}>
            <div className={styles.inputGroup}>
              <label htmlFor="apTest">AP Test:</label>
              <select
                id="apTest"
                value={testName}
                onChange={handleTestChange}
                className={styles.input}
              >
                {apTests.map((test) => (
                  <option key={test} value={test}>
                    {test}
                  </option>
                ))}
              </select>
            </div>

            <div className={styles.inputGroup}>
              <label htmlFor="apScore">AP Test Score (1-5):</label>
              <input
                type="number"
                id="apScore"
                value={testScore}
                onChange={(e) => {
                  const numericValue = Number(e.target.value);
                  if (numericValue >= 1 && numericValue <= 5) {
                    e.target.value = numericValue.toString();
                    setTestScore(numericValue);
                  } else if (e.target.value === "") {
                    setTestScore(0); // Reset to 0 if input is cleared
                  }
                }}
                className={styles.input}
                min="1"
                max="5"
                maxLength={1}
              />
            </div>

          </div>

          <div className={styles.results}>
            {earnedCredit ?? <p>Please select a test and enter a valid score.</p>}
          </div>

          <button onClick={handleAddTestToAccount} className={styles.button}>
            Add AP Test to Account
          </button>
        </div>
      </div>
    </>
  );
}