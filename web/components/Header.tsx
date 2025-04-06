"use client";

import Link from "next/link";
import { useRouter } from "next/navigation";
import { useEffect, useState } from "react";
import styles from "../app/page.module.css";

export default function Header() {
  const [username, setUsername] = useState<string | null>(null);
  const router = useRouter(); // Initialize router

  useEffect(() => {
    const currentUser = localStorage.getItem("currentUser");
    if (currentUser) {
      setUsername(currentUser);
    }
  }, []);

  const handleLogout = () => {
    localStorage.removeItem("currentUser"); // Remove user from localStorage
    setUsername(null); // Clear username state
    router.push("/"); // Redirect to the homepage
  };

  return (
    <header className={styles.header}>
      <Link href="/" className={styles.logo}>
        creg
      </Link>
      <nav className={styles.nav}>
        <Link href="/first-years" className={styles.navButton}>
          First Year Information
        </Link>
        <Link href="/register-landing-page" className={styles.navButton}>
          Course Registration
        </Link>
        <Link href="/ap-credit" className={styles.navButton}>
          AP Credit
        </Link>
        <Link href="/pathways" className={styles.navButton}>
          Pathways
        </Link>
        {username ? (
          <Link href="/" onClick={handleLogout} className={styles.navButton}>
            Logout
          </Link>
        ) : (
          <Link href="/login" className={styles.navButton}>
            Login
          </Link>
        )}
        {username && <span className={styles.username}>{username}</span>}
      </nav>
    </header>
  );
}