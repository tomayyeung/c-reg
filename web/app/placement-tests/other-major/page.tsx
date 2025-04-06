"use client";

import { useState, useEffect } from "react";
import Header from "@/components/Header";
import ChatBot from "@/components/ChatBot";
import styles from "../page.module.css";

/*
  Math Placement Test
  - 76-100%: MATH 109: Calculus & Analytic Geom I
  - 68-75%: MATH 108: Precalculus, but you may retake the test to try to place into MATH 109
  - 61-67%: MATH 107: Calculus for the Liberal Arts, MATH 108: Precalculus
  - 46-60%: MATH 105: Mathematics for Educators
  - 45% or below: MATH 104: Algebra for Business & Science
*/

export default function GenericPathway() {
  const [mathScore, setMathScore] = useState<number>(0); // Default to 0
  const [mathRecommendation, setMathRecommendation] = useState<string | null>(null);

  useEffect(() => {
    if (mathScore !== null) {
      if (mathScore >= 76) setMathRecommendation("MATH 109: Calculus & Analytic Geom I");
      else if (mathScore >= 68) setMathRecommendation("MATH 108: Precalculus or retake the test to place into MATH 109: Calculus & Analytic Geom I (recommended)");
      else if (mathScore >= 61) setMathRecommendation("MATH 108: Precalculus");
      else if (mathScore >= 46) setMathRecommendation("MATH 105: Mathematics for Educators");
      else setMathRecommendation("MATH 104: Algebra for Business & Science");
    } else {
      setMathRecommendation(null);
    }
  }, [mathScore]);

  const handleMathScoreChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;

    // Remove leading zeros
    if (value.startsWith("0") && value.length > 1) {
      e.target.value = value.slice(1);
    }

    const numericValue = Number(value);
    if (numericValue >= 0 && numericValue <= 100) {
      setMathScore(numericValue);
    }
  };

  return (
    <>
      <Header />
      <ChatBot />

      <div className={styles.container}>
        <div className={styles.main}>
          <div className={styles.hero}>
            <h1 className={styles.title}>Math Course Pathway</h1>
            <p className={styles.subtitle}>
              Enter your placement test results to see your recommended pathways.
            </p>
          </div>

          <div className={styles.form}>
            <div className={styles.inputGroup}>
              <label htmlFor="mathScore">Math Placement Test Score:</label>
              <input
                type="number"
                id="mathScore"
                value={mathScore}
                onChange={handleMathScoreChange}
                className={styles.input}
              />
            </div>
          </div>

          <div className={styles.results}>
            {mathRecommendation ? (
              <p> <strong>Math Recommendation:</strong> {mathRecommendation}  </p>
            ) : (
              <p>Please enter your score above to see your recommendation.</p>
            )}
          </div>

        </div>
      </div>
    </>
  );
}