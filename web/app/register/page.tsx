import Header from "@/components/Header";
import ChatBot from "@/components/ChatBot";
import styles from "../first-years/page.module.css";

export default function Register() {
  return (
    <>
      <Header />
      <ChatBot />

      <div className={styles.container}>
        <div className={styles.main}>
          <div className={styles.hero}>
            <h1 className={styles.title}>Register</h1>
            <p className={styles.subtitle}>
              Coming Soon :)
              <br></br> 👍
            </p>
          </div>
        </div>
      </div>
    </>
  );
}