import Header from "@/components/Header";
import ChatBot from "@/components/ChatBot";
import styles from "../first-years/page.module.css";
import Link from "next/link";

export default function Pathways() {
  return (
    <>
      <Header />
      <ChatBot />

      <div className={styles.container}>
        <div className={styles.main}>
          <div className={styles.hero}>
            <h1 className={styles.title}>Pathways</h1>
            <p className={styles.subtitle}>
              Enter your placement test results to see your recommended pathways.
            </p>
          </div>

          <h2>Select your major:</h2>

          <ul className={styles.majorList} style={{ gridTemplateColumns: "repeat(1, 1fr)" }}>

            <li className={styles.majorItem}>
              <Link href="https://www.youtube.com/watch?v=3sUDzwGaPrY" className={styles.majorLink}>
                <span className={styles.majorName}>Chemistry</span>
              </Link>
            </li>

            <li className={styles.majorItem}>
              <Link href="/pathways/cs" className={styles.majorLink}>
                <span className={styles.majorName}>Computer Science</span>
              </Link>
            </li>

          </ul>
        </div>
      </div >
    </>
  );
}