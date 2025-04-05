import Link from "next/link";
import styles from "./page.module.css";

export default function Home() {
  return (
    <div className={styles.page}>
      <header className={styles.header}>
        <div className={styles.container}>
          <Link href="/" className={styles.logo}>
            creg
          </Link>
          <nav className={styles.nav}>
            <Link href="/first-years" className={styles.navButton}>
              First Year Information
            </Link>
            <Link href="/registration" className={styles.navButton}>
              Course Registration
            </Link>
          </nav>
        </div>
      </header>

      <main className={styles.main}>
        <div className={styles.container}>
          <div className={styles.hero}>
            <h1 className={styles.title}>Welcome to creg</h1>
            <p className={styles.subtitle}>
              Your central hub for all course registration needs
            </p>
            <div className={styles.ctas}>
              <Link href="/registration" className={styles.primaryCta}>
                Register for Courses
              </Link>
              <Link href="/first-years" className={styles.secondaryCta}>
                First Year Guide
              </Link>
            </div>
          </div>
        </div>
      </main>
    </div>
  );
}