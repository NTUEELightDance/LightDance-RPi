module.exports = {
    env: {
        browser: true,
        es2020: true,
    },
    extends: ["airbnb", "plugin:prettier/recommended"],
    parserOptions: {
        ecmaVersion: 11,
        sourceType: "module",
    },
    plugins: ["prettier"],
    rules: {
        "prettier/prettier": ["error", { endOfLine: "auto" }],
    },
};
