module.exports = {
    env: {
        node: true,
        es6: true,
        'jest/globals': true,
    },
    plugins: ['@typescript-eslint', 'jest'],
    extends: ['eslint:recommended', 'plugin:@typescript-eslint/recommended', 'plugin:jest/recommended'],
    parser: '@typescript-eslint/parser',
    parserOptions: {
        ecmaVersion: 12,
    },
    rules: {
        '@typescript-eslint/no-explicit-any': 'off',
        '@typescript-eslint/no-var-requires': 'off',
    },
};
