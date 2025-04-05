"use client";

import { useEffect, useState } from "react";
import Header from "@/components/Header";
import { useRouter } from "next/navigation";
import Link from "next/link";
import Image from "next/image";
import styles from "./page.module.css";

export default function RegisterPage() {
  const [username, setUsername] = useState<string | null>(null);
  const [loading, setLoading] = useState(true);
  const router = useRouter();

  useEffect(() => {
    // Check if user is logged in
    const currentUser = localStorage.getItem("currentUser");
    if (!currentUser) {
      router.push("/");
      return;
    }

    setUsername(currentUser);
    setLoading(false);
  }, [router]);

  if (loading) {
    return (
      <div className="flex min-h-screen items-center justify-center">
        <div className="text-center">
          <div className="mb-4 h-12 w-12 animate-spin rounded-full border-4 border-blue-600 border-t-transparent mx-auto"></div>
          <p>Loading...</p>
        </div>
      </div>
    );
  }

  return (
    <>
      <Header />
      <div className={styles.pageLayout}>
        <div className={styles.sideBar}>
          <Image
            src="/USFCA logo.png"
            alt="Background"
            className={styles.heroImage}
            width={200}
            height={200}
            priority
          />
          <div className={styles.header}>
            <h1 className={styles.title}>Course Registration Portal</h1>
            <p className={styles.subtitle}>Welcome, {username}</p>
          </div>
          <button
            onClick={() => {
              localStorage.removeItem("currentUser");
              router.push("/");
            }}
            className={styles.logoutButton}
          >
            Logout
          </button>
        </div>
        
        <div className={styles.mainContent}>
          <div className={styles.grid}>
            {/* Prepare for Registration */}
            <Link href="/register/prepare" className={styles.card}>
              <div className={styles.iconContainer}></div>
              <div className={styles.cardContent}>
                <h2 className={styles.cardTitle}>Prepare for Registration</h2>
                <p className={styles.cardSubtitle}>For Students only</p>
                <p className={styles.cardDescription}>
                  View registration status and permit overrides.
                </p>
              </div>
            </Link>

            {/* Register for Classes */}
            <Link href="/register/classes" className={styles.card}>
              <div className={styles.iconContainer}></div>
              <div className={styles.cardContent}>
                <h2 className={styles.cardTitle}>Register for Classes</h2>
                <p className={styles.cardSubtitle}>For Students only</p>
                <p className={styles.cardDescription}>
                  Search and register for your classes. You can also view and
                  manage your schedule.
                </p>
              </div>
            </Link>

            {/* Plan Ahead */}
            <Link href="/register/plan-ahead" className={styles.card}>
              <div className={styles.iconContainer}></div>
              <div className={styles.cardContent}>
                <h2 className={styles.cardTitle}>Plan Ahead</h2>
                <p className={styles.cardSubtitle}>For Students only</p>
                <p className={styles.cardDescription}>
                  Give yourself a head start by building plans. When you're
                  ready to register, you'll be able to load these plans.
                </p>
              </div>
            </Link>

            {/* Browse Classes */}
            <Link href="/register/browse-classes" className={styles.card}>
              <div className={styles.iconContainer}></div>
              <div className={styles.cardContent}>
                <h2 className={styles.cardTitle}>Browse Classes</h2>
                <p className={styles.cardSubtitle}>
                  For Students, Faculty, Advisers, and Public
                </p>
                <p className={styles.cardDescription}>
                  Looking for classes? In this section you can browse classes
                  you find interesting.
                </p>
              </div>
            </Link>

            {/* View Registration Information */}
            <Link href="/register/view-information" className={styles.card}>
              <div className={styles.iconContainer}></div>
              <div className={styles.cardContent}>
                <h2 className={styles.cardTitle}>
                  View Registration Information
                </h2>
                <p className={styles.cardSubtitle}>For Students only</p>
                <p className={styles.cardDescription}>
                  View your past schedules and your upgraded classes.
                </p>
              </div>
            </Link>

            {/* Browse Course Catalog */}
            <Link href="/register/course-catalog" className={styles.card}>
              <div className={styles.iconContainer}></div>
              <div className={styles.cardContent}>
                <h2 className={styles.cardTitle}>Browse Course Catalog</h2>
                <p className={styles.cardSubtitle}>
                  For Students, Faculty, Advisers, and Public
                </p>
                <p className={styles.cardDescription}>
                  Look up basic course information like subject, course and
                  description.
                </p>
              </div>
            </Link>
          </div>
        </div>
      </div>
    </>
  );
}
