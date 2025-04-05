import "./globals.css";
import styles from "./page.module.css";
import type { Metadata } from "next";
import { ChatProvider } from "../context/ChatContext";

export const metadata: Metadata = {
  title: 'creg',
  description: 'Your central hub for all course registration needs',
}

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body>
        <div className={styles.page}>
          <main>
            <ChatProvider>{children}</ChatProvider>
          </main>
        </div>
      </body>
    </html>
  )
}