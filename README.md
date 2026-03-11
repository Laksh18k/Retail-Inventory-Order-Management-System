# Retail Inventory & Order Management System

A robust, high-performance Command Line Interface (CLI) application built in C to manage retail operations. This system efficiently handles a catalog of over 3,000 SKUs and 5,000+ orders, providing persistent storage, advanced sorting, and business intelligence features like ABC Analysis.

## 🚀 Features

* **Advanced Inventory Management:** Perform CRUD operations on a vast catalog of SKUs. Automatically track stock levels and update sold counts upon order delivery.
* **Order Processing Lifecycle:** Place, deliver, and cancel orders with automated status tracking. Includes randomized delivery time simulation for realistic order fulfillment.
* **ABC Analysis (Pareto Principle):** Categorize inventory into Class A (Top 80% of sales), Class B (Next 15%), and Class C (Bottom 5%) to optimize stock management and identify best-selling products.
* **High-Performance Sorting:** Utilizes a custom implementation of the **QuickSort algorithm** to rank products by sales volume and organize order histories chronologically.
* **Persistent CSV Storage:** Seamlessly imports and exports data to `.csv` files (`Import_Store.csv`, `Orders.csv`, `OrderDetails.csv`), ensuring no data is lost between sessions.

## 🛠️ Tech Stack & Tools

* **Language:** C
* **Core Concepts:** Custom Data Structures (`structs`), Memory Management, File Handling (I/O), Pointer Manipulation
* **Algorithms:** QuickSort, Time Complexity Optimization
* **Compiler:** GCC

## 🧠 Data Structures & Algorithms Highlights

* **Structs:** Designed distinct structures for `sku` and `Order` to tightly pack data and efficiently manage thousands of records in memory.
* **QuickSort:** Implemented an optimized QuickSort with a custom comparator to sort SKUs based on `soldCount` and resolve ties alphabetically.
* **Date/Time Hashing:** Converts standard `time_t` dates into a `long long` integer format `(YYYYMMDDHHMM)` for O(1) constant-time comparison during order sorting.

## ⚙️ How to Compile and Run

Make sure you have a C compiler (like GCC) installed on your system.

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/Laksh18k/Retail-Inventory-Order-Management-System.git](https://github.com/Laksh18k/Retail-Inventory-Order-Management-System.git)
   cd Retail-Inventory-Order-Management-System
