"use client"

import { useState, useEffect } from "react"
import { useRouter } from "next/navigation"
import styles from "./page.module.css"

interface Section {
  _id: string
  course: string
  section_num: number
  crn: number
  schedule_type: string
  campus: string
  title: string
  instruction_mode: number
  meeting_type: string
  days: string
  begin_time: string
  end_time: string
  start_date: string
  end_date: string
  building: string
  room: string
  capacity: number
  enrollment: number
  instructor_first: string
  instructor_last: string
  instructor_email: string
  college: string
}

export default function RegisterForClasses() {
  const [sections, setSections] = useState<Section[]>([])
  const [searchTerm, setSearchTerm] = useState("")
  const [sortBy, setSortBy] = useState("course")
  const [selectedSections, setSelectedSections] = useState<Section[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState("")
  const [successMessage, setSuccessMessage] = useState("")
  const [currentPage, setCurrentPage] = useState(1)
  const [totalPages, setTotalPages] = useState(1)
  const pageSize = 10 // Number of sections per page
  const router = useRouter()

  useEffect(() => {
    const currentUser = localStorage.getItem("currentUser")
    if (!currentUser) {
      router.push("/login")
      return
    }

    // Initial fetch with default parameters
    fetchSections(searchTerm, sortBy, 1)
  }, [router])

  // Debounce search to avoid too many API calls
  useEffect(() => {
    const timer = setTimeout(() => {
      fetchSections(searchTerm, sortBy, 1) // Reset to page 1 when search or sort changes
      setCurrentPage(1)
    }, 300)

    return () => clearTimeout(timer)
  }, [searchTerm, sortBy])

  const fetchSections = async (search: string, sort: string, page: number) => {
    try {
      setLoading(true)

      // Build query parameters
      const params = new URLSearchParams()
      if (search) params.append("search", search)
      if (sort) params.append("sort", sort)
      params.append("page", page.toString())
      params.append("pageSize", pageSize.toString())

      const response = await fetch(`/api/sections?${params.toString()}`)
      if (!response.ok) {
        throw new Error("Failed to fetch sections")
      }
      const data = await response.json()
      setSections(data.sections)
      setTotalPages(data.totalPages)
    } catch (err) {
      setError("Error loading sections. Please try again later.")
      console.error(err)
    } finally {
      setLoading(false)
    }
  }

  const handlePageChange = (newPage: number) => {
    if (newPage < 1 || newPage > totalPages) return
    setCurrentPage(newPage)
    fetchSections(searchTerm, sortBy, newPage)
  }

  const handleAddSection = (section: Section) => {
    if (selectedSections.some((s) => s._id === section._id)) {
      setError("This section is already in your selection.")
      setTimeout(() => setError(""), 3000)
      return
    }
    console.log("adding section" + section)
    setSelectedSections([...selectedSections, section])
  }

  const handleRemoveSection = (sectionId: string) => {
    setSelectedSections(selectedSections.filter((s) => s._id !== sectionId))
  }

  const handleEnroll = async () => {
    if (selectedSections.length === 0) {
      setError("Please select at least one class to enroll.")
      setTimeout(() => setError(""), 3000)
      return
    }

    try {
      const currentUser = localStorage.getItem("currentUser")
      if (!currentUser) {
        router.push("/login")
        return
      }

      const enrollmentData = {
        username: currentUser,
        sections: selectedSections.map((s) => s._id),
      }

      const response = await fetch("/api/enrollments", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(enrollmentData),
      })

      if (!response.ok) {
        throw new Error("Failed to enroll in classes")
      }

      setSuccessMessage("Successfully enrolled in classes!")
      setSelectedSections([])
      setTimeout(() => setSuccessMessage(""), 3000)
    } catch (err) {
      setError("Error enrolling in classes. Please try again.")
      console.error(err)
      setTimeout(() => setError(""), 3000)
    }
  }

  const formatTime = (time: string | number | null | undefined) => {
    if (!time) return ""
  
    const timeStr = time.toString().padStart(4, "0") // ensure it’s at least 4 digits
  
    const hour = Number.parseInt(timeStr.substring(0, 2))
    const minute = timeStr.substring(2)
    const period = hour >= 12 ? "PM" : "AM"
    const formattedHour = hour > 12 ? hour - 12 : hour === 0 ? 12 : hour
  
    return `${formattedHour}:${minute} ${period}`
  }

  const formatDays = (days: string) => {
    if (!days || days === "MW") return days
    const dayMap: Record<string, string> = {
      M: "Mon",
      T: "Tue",
      W: "Wed",
      R: "Thu",
      F: "Fri",
      S: "Sat",
      U: "Sun",
    }

    return days
      .split("")
      .map((day) => dayMap[day] || day)
      .join(", ")
  }

  return (
    <div className={styles.container}>
      <h1 className={styles["page-title"]}>Register for Classes</h1>

      {error && (
        <div className={styles["error-alert"]}>
          <p>{error}</p>
        </div>
      )}

      {successMessage && (
        <div className={styles["success-alert"]}>
          <p>{successMessage}</p>
        </div>
      )}

      <div className={styles["registration-layout"]}>
        <div className={styles["sections-container"]}>
          <div className={styles.card}>
            <div className={styles["card-header"]}>
              <h2 className={styles["card-title"]}>Available Sections</h2>
              <div className={styles["search-controls"]}>
                <div className={styles["search-input-container"]}>
                  <input
                    type="text"
                    placeholder="Search courses..."
                    className={styles["search-input"]}
                    value={searchTerm}
                    onChange={(e) => setSearchTerm(e.target.value)}
                  />
                </div>
              </div>
            </div>
            <div className={styles["card-content"]}>
              {loading ? (
                <div className={styles["loading-message"]}>Loading sections...</div>
              ) : sections.length === 0 ? (
                <div className={styles["empty-message"]}>No sections found matching your search.</div>
              ) : (
                <div className={styles["sections-list"]}>
                  {sections.map((section) => (
                    <div key={section._id} className={styles["section-card"]}>
                      <div className={styles["section-details"]}>
                        <div className={styles["section-header"]}>
                          <div>
                            <h3 className={styles["section-title"]}>{section.title}</h3>
                            <p className={styles["section-course-info"]}>
                              {section.course} - Section {section.section_num} - CRN: {section.crn}
                            </p>
                          </div>
                          <div
                            className={`${styles["enrollment-badge"]} ${section.enrollment <= section.capacity ? styles.full : ""}`}
                          >
                           {section.capacity}/{section.enrollment} Enrolled
                          </div>
                        </div>
                        <div className={styles["section-info-grid"]}>
                          <div>
                            <p className={styles["section-info"]}>
                              <span className={styles["info-label"]}>Instructor:</span> {section.instructor_first}{" "}
                              {section.instructor_last}
                            </p>
                            <p className={styles["section-info"]}>
                              <span className={styles["info-label"]}>Location:</span> {section.building} {section.room}
                            </p>
                          </div>
                          <div>
                            <p className={styles["section-info"]}>
                              <span className={styles["info-label"]}>Days:</span> {formatDays(section.days)}
                            </p>
                            <p className={styles["section-info"]}>
                              <span className={styles["info-label"]}>Time:</span> {formatTime(section.begin_time)} -{" "}
                              {formatTime(section.end_time)}
                            </p>
                          </div>
                        </div>
                      </div>
                      <div className={styles["section-actions"]}>
                        <button
                          className={`${styles["add-button"]} ${section.enrollment <= section.capacity || selectedSections.some((s) => s._id === section._id) ? styles.disabled : ""}`}
                          onClick={() => handleAddSection(section)}
                          disabled={
                            section.enrollment <= section.capacity ||
                            selectedSections.some((s) => s._id === section._id)
                          }
                        >
                          Add to Selection
                        </button>
                      </div>
                    </div>
                  ))}
                </div>
              )}

              {totalPages > 1 && (
                <div className={styles.pagination}>
                  <button
                    className={styles["pagination-button"]}
                    onClick={() => handlePageChange(currentPage - 1)}
                    disabled={currentPage === 1}
                  >
                    Previous
                  </button>
                  <span className={styles["page-info"]}>
                    Page {currentPage} of {totalPages}
                  </span>
                  <button
                    className={styles["pagination-button"]}
                    onClick={() => handlePageChange(currentPage + 1)}
                    disabled={currentPage === totalPages}
                  >
                    Next
                  </button>
                </div>
              )}
            </div>
          </div>
        </div>

        <div className={styles["selection-container"]}>
          <div className={`${styles.card} ${styles["selection-card"]}`}>
            <div className={styles["card-header"]}>
              <h2 className={styles["card-title"]}>Selected Classes</h2>
            </div>
            <div className={styles["card-content"]}>
              {selectedSections.length === 0 ? (
                <div className={styles["empty-message"]}>
                  No classes selected yet. Browse and add classes from the list.
                </div>
              ) : (
                <div className={styles["selected-sections-list"]}>
                  {selectedSections.map((section) => (
                    <div key={section._id} className={styles["selected-section"]}>
                      <div>
                        <p className={styles["selected-section-course"]}>{section.course}</p>
                        <p className={styles["selected-section-title"]}>{section.title}</p>
                      </div>
                      <button className={styles["remove-button"]} onClick={() => handleRemoveSection(section._id)}>
                        Remove
                      </button>
                    </div>
                  ))}
                </div>
              )}
            </div>
            <div className={styles["card-footer"]}>
              <button
                className={`${styles["enroll-button"]} ${selectedSections.length === 0 ? styles.disabled : ""}`}
                onClick={handleEnroll}
                disabled={selectedSections.length === 0}
              >
                Enroll in Selected Classes
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  )
}

