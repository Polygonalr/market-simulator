class Market {
    public:
        Market();
        ~Market();
    private:
        std::unordered_map<std::string, Book> order_books;
};