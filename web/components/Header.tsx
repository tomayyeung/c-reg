import Link from "next/link";
import styles from "../app/page.module.css";

export default function Header() {
  return (
    <header className={styles.header}>
      <div className={styles.container}>
        <Link href="/" className={styles.logo}>
          creg
        </Link>
        <nav className={styles.nav}>
          <Link href="/first-years" className={styles.navButton}>
            First Year Information
          </Link>
          <Link href="/register" className={styles.navButton}>
            Course Registration
          </Link>
          <Link href="/pathways" className={styles.navButton}>
            Pathways
          </Link>
        </nav>
      </div>
    </header>
  );
}