import Link from "next/link";
import Header from "../../components/Header";
import styles from "./page.module.css";

export default function FirstYears() {
  return (
    <>
      <Header />

      <div className={styles.container}>
        <div className={styles.main}>
          <div className={styles.hero}>
            <h1 className={styles.title}>First Year Student Guide</h1>
            <p className={styles.subtitle}>
              Everything you need to know to start your journey at USF
            </p>
          </div>

          <div className={styles.infoSection}>
            <h2 className={styles.sectionTitle}>Getting Started</h2>
            <div className={styles.infoGrid}>
              <div className={styles.infoCard}>
                <h3>Watch the University Graduation Requirements videos</h3>
                <p>Watch two short videos on the graduation requirements and core curriculum.</p>
                <Link href="/first-years/university-requirements" className={styles.cardLink}>Watch Video</Link>
              </div>

              <div className={styles.infoCard}>
                <h3>Watch Your Major-Specific Graduation Requirements Video</h3>
                <p>Learn about the specific requirements and recommendations for your chosen major and see example schedules.</p>
                <Link href="/first-years/major-requirements" className={styles.cardLink}>Watch Video</Link>
              </div>

              <div className={styles.infoCard}>
                <h3>Course Requirements</h3>
                <p>Understand your degree requirements, general education courses, and first-year recommendations.</p>
                <Link href="/requirements" className={styles.cardLink}>View Requirements</Link>
              </div>

              <div className={styles.infoCard}>
                <h3>Important Dates</h3>
                <p>Mark your calendar with registration periods, add/drop deadlines, and academic calendar milestones.</p>
                <Link href="https://www.usfca.edu/academic-calendar" className={styles.cardLink} target="_blank" rel="noopener noreferrer">View Calendar</Link>
              </div>
            </div>
          </div>
        </div>
      </div>
    </>
  );
}